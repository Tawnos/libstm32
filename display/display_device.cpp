#include "display_device.h"
#include "../rgbcolor.h"
#include "../config.h"
#include "../logger.h"
#include "assert.h"
#include <string.h>
//#include <diag/Trace.h>

namespace cmdc0de {
	//////////////////////////////////////////////////////////////////////

	bool DrawBufferNoBuffer::drawPixel(uint16_t x0, uint16_t y0, const RGBColor& color) {
		PackedColor pc = PackedColor::create(getPixelFormat(), color);
		setAddrWindow(x0, y0, x0, y0);
		return writeNData(pc.getPackedColorData(), pc.getSize());
	}

	void DrawBufferNoBuffer::drawImage(int16_t x, int16_t y, const DCImage& dc) {
		setAddrWindow(0, 0, dc.width, dc.height);
		writeNData((const uint8_t*)&dc.pixel_data[0], dc.height * dc.width * dc.bytes_per_pixel);
	}

	void DrawBufferNoBuffer::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
		setAddrWindow(x, y, w, h);
		PackedColor pc = PackedColor::create(getPixelFormat(), color);

		uint16_t pcolor = *((uint16_t*)(pc.getPackedColorData()));
		uint16_t pixelCount = w * h;
		uint16_t maxAtOnce =
			pixelCount > (RowsForDrawBuffer * getDisplay()->getWidth()) ?
			(RowsForDrawBuffer * getDisplay()->getWidth()) : pixelCount;
		for (uint16_t i = 0; i < maxAtOnce; ++i) {
			SPIBuffer[i] = pcolor;
		}

		uint16_t pixelCopied = 0;
		do {
			writeNData((uint8_t*)&SPIBuffer[0], maxAtOnce * sizeof(uint16_t));
			pixelCopied += maxAtOnce;

			if ((pixelCopied + maxAtOnce) > pixelCount) {
				maxAtOnce = pixelCount - pixelCopied;
			}
		} while (pixelCopied < pixelCount);
	}

	void DrawBufferNoBuffer::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) {
		PackedColor pc = PackedColor::create(getPixelFormat(), color);

		uint16_t pcolor = *((uint16_t*)(pc.getPackedColorData()));
		uint16_t pixelCount = h;
		uint16_t maxAtOnce =
			pixelCount > (RowsForDrawBuffer * getDisplay()->getWidth()) ?
			(RowsForDrawBuffer * getDisplay()->getWidth()) : pixelCount;
		for (uint16_t i = 0; i < maxAtOnce; ++i) {
			SPIBuffer[i] = pcolor;
		}

		uint16_t pixelCopied = 0;
		setAddrWindow(x, y, x, y + h - 1);
		do {
			writeNData((uint8_t*)&SPIBuffer[0], maxAtOnce * sizeof(uint16_t));
			pixelCopied += maxAtOnce;

			if ((pixelCopied + maxAtOnce) > pixelCount) {
				maxAtOnce = pixelCount - pixelCopied;
			}
		} while (pixelCopied < pixelCount);
	}

	void DrawBufferNoBuffer::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
		PackedColor pc = PackedColor::create(getPixelFormat(), color);

		uint16_t pcolor = *((uint16_t*)(pc.getPackedColorData()));
		for (uint16_t i = 0; i < w; ++i) {
			SPIBuffer[i] = pcolor;
		}

		setAddrWindow(x, y, x + w - 1, y);
		writeNData((uint8_t*)&SPIBuffer[0], w * sizeof(uint16_t));
	}

	bool DrawBuffer2D16BitColor::drawPixel(uint16_t x, uint16_t y, const RGBColor& color) {
		uint8_t c = deresColor(color);
		BackBuffer.setValueAsByte((y * Width) + x, c);
		DrawBlocksChanged.setValueAsByte(y / RowsForDrawBuffer, 1);
		return true;
	}

	//not using buffer just write directly to SPI
	void DrawBuffer2D16BitColor::drawImage(int16_t x, int16_t y, const DCImage& dc) {
		setAddrWindow(0, 0, dc.width - 1, dc.height - 1);
		writeNData((const uint8_t*)&dc.pixel_data[0], dc.height * dc.width * dc.bytes_per_pixel);
		DrawBlocksChanged.clear();
	}

	void DrawBuffer2D16BitColor::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
		uint8_t c = deresColor(color);
		for (int i = y; i < (h + y); ++i) {
			//OPTIMIZE THIS BY MAKING A SET RANGE IN BITARRAY
			uint32_t offset = i * getDisplay()->getWidth();
			for (int j = 0; j < w; ++j) {
				BackBuffer.setValueAsByte(offset + x + j, c);
			}
			DrawBlocksChanged.setValueAsByte(i / RowsForDrawBuffer, 1);
		}
	}

	void DrawBuffer2D16BitColor::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) {
		uint8_t c = deresColor(color);
		for (int i = y; i < (h + y); ++i) {
			BackBuffer.setValueAsByte(i * getDisplay()->getWidth() + x, c);
			DrawBlocksChanged.setValueAsByte(i / RowsForDrawBuffer, 1);
		}
	}

	void DrawBuffer2D16BitColor::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
		uint8_t c = deresColor(color);
		uint32_t offset = y * getDisplay()->getWidth();
		for (int i = x; i < (x + w); ++i) {
			BackBuffer.setValueAsByte(offset + i, c);
		}
		DrawBlocksChanged.setValueAsByte(y / RowsForDrawBuffer, 1);
	}

	//////
	// first check to see if we changed anything in the draw block, if not skip it
	// if we did change something convert from our short hand notation to something the LCD will understand
	//	then send to LCD
	void DrawBuffer2D16BitColor::swap() {
		for (int h = 0; h < Height; h++) {
			if ((DrawBlocksChanged.getValueAsByte(h / RowsForDrawBuffer)) != 0) {
				for (int w = 0; w < Width; w++) {
					uint32_t SPIY = h % RowsForDrawBuffer;
					SPIBuffer[(SPIY * Width) + w] = calcLCDColor(BackBuffer.getValueAsByte((h * Width) + w));
				}
				if (h != 0 && (h % RowsForDrawBuffer == (RowsForDrawBuffer - 1))) {
					setAddrWindow(0, h - (RowsForDrawBuffer - 1), Width, h);
					writeNData((uint8_t*)&SPIBuffer[0], Width * RowsForDrawBuffer * sizeof(uint16_t));
				}
			}
		}
		DrawBlocksChanged.clear();
	}

	uint16_t DrawBuffer2D16BitColor::calcLCDColor(uint8_t packedColor) {
		static const uint8_t colorValues[4] = { 0,85,170,255 };
		uint32_t rc = (packedColor & RED_MASK) >> 4;
		uint32_t gc = (packedColor & GREEN_MASK) >> 2;
		uint32_t bc = packedColor & BLUE_MASK;
		RGBColor lcdColor(colorValues[rc], colorValues[gc], colorValues[bc]);
		uint8_t* packedData = PackedColor::create(getPixelFormat(), lcdColor).getPackedColorData();
		uint16_t* pcd = (uint16_t*)packedData;
		return *pcd;
	}
	uint8_t DrawBuffer2D16BitColor::deresColor(const RGBColor& color) {
		uint32_t retVal = 0;
		retVal = (color.getR() / 85) << 4;
		retVal |= (color.getG() / 85) << 2;
		retVal |= (color.getB() / 85);
		return retVal;
	}


	/////////////////////////////////////////

	bool DrawBuffer2D16BitColor16BitPerPixel1Buffer::drawPixel(uint16_t x, uint16_t y, const RGBColor& color) {
		SPIBuffer[(y * Width) + x] = calcLCDColor(color);
		return true;
	}

	//not using buffer just write directly to SPI
	void DrawBuffer2D16BitColor16BitPerPixel1Buffer::drawImage(int16_t x1, int16_t y1, const DCImage& dc) {
#if 0
		setAddrWindow(0, 0, dc.width, dc.height);
		writeNData((const uint8_t*)&dc.pixel_data[0], dc.height * dc.width * dc.bytes_per_pixel);
#else
		uint16_t* t = (uint16_t*)&dc.pixel_data[0];
		for (auto y = 0; y < dc.height; ++y) {
			for (int x = 0; x < dc.width; ++x) {
				RGBColor c(((t[(y * dc.height) + x] & 0b1111100000000000) >> 11),
					((t[(y * dc.height) + x] & 0b0000011111100000) >> 5),
					((t[(y * dc.height) + x] & 0b0000000000011111)));
				uint8_t* packedData = PackedColor::create(getPixelFormat(), c).getPackedColorData();
				uint16_t* pcd = (uint16_t*)packedData;
				SPIBuffer[((y + y1) * Width) + (x + x1)] = (*pcd);
			}
		}
#endif
	}

	void DrawBuffer2D16BitColor16BitPerPixel1Buffer::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
		uint16_t c = calcLCDColor(color);
		for (int i = y; i < (h + y); ++i) {
			//OPTIMIZE THIS BY MAKING A SET RANGE IN BITARRAY
			uint32_t offset = i * getDisplay()->getWidth();
			for (int j = 0; j < w; ++j) {
				SPIBuffer[offset + x + j] = c;
			}
		}
	}

	void DrawBuffer2D16BitColor16BitPerPixel1Buffer::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) {
		uint16_t c = calcLCDColor(color);
		for (int i = y; i < (h + y); ++i) {
			SPIBuffer[i * getDisplay()->getWidth() + x] = c;
		}
	}

	void DrawBuffer2D16BitColor16BitPerPixel1Buffer::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
		uint16_t c = calcLCDColor(color);
		uint32_t offset = y * getDisplay()->getWidth();
		for (int i = x; i < (x + w); ++i) {
			SPIBuffer[offset + i] = c;
		}
	}

	void DrawBuffer2D16BitColor16BitPerPixel1Buffer::swap() {
		if (/*anychange*/ 1) {
			setAddrWindow(0, 0, Width, Height);
			writeNData((uint8_t*)&SPIBuffer[0], BufferSize);
		}
	}

	uint16_t DrawBuffer2D16BitColor16BitPerPixel1Buffer::calcLCDColor(const RGBColor& color) {
		uint32_t rc = color.getR() / 8; //keep it in 5 bits
		uint32_t gc = color.getG() / 4; //keep it in 6 bits
		uint32_t bc = color.getB() / 8; //keep it in 5 bits
		RGBColor lcdColor(rc, gc, bc);
		uint8_t* packedData = PackedColor::create(getPixelFormat(), lcdColor).getPackedColorData();
		uint16_t* pcd = (uint16_t*)packedData;
		return *pcd;
	}
}