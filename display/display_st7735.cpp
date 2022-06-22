#include "display_st7735.h"
using namespace cmdc0de;

///////////////////////////////////////////////////////////////////////
void DisplayST7735::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	//if ((MemoryAccessControl & DisplayST7735::MADCTL_MY) == 0) {
	//	writeCmd(DisplayST7735::COLUMN_ADDRESS_SET);
	//	write16Data(y0);
	//	write16Data(y1);

	//	writeCmd(DisplayST7735::ROW_ADDRESS_SET);
	//	write16Data(x0);
	//	write16Data(x1);
	//} else {
	//this code thinks about everything as x is columsn y is rows
	writeCmd(DisplayST7735::COLUMN_ADDRESS_SET);
	write16Data(x0);
	write16Data(x1);

	writeCmd(DisplayST7735::ROW_ADDRESS_SET);
	write16Data(y0);
	write16Data(y1);
	//}
}

void DisplayST7735::setMemoryAccessControl() {
	uint8_t macctl = 0;
	switch (Display->getRotation()) {
	case LandscapeTopLeft:
		macctl = DisplayST7735::MADCTL_MV | DisplayST7735::MADCTL_MX;
		break;
	case PortraitTopLeft:
	default:
		break;
	}
	if (!Display->isTopToBotRefresh()) {
		macctl |= DisplayST7735::MADCTL_VERTICAL_REFRESH_ORDER_BOT_TOP;
	}

	if (macctl != MemoryAccessControl) {
		MemoryAccessControl = macctl;
		////TODO Can't swap between landscape and portait
		//Display->reset();
		//MemoryAccessControl = DisplayST7735::MADCTL_MV|DisplayST7735::MADCTL_MX|DisplayST7735::MADCTL_VERTICAL_REFRESH_ORDER_BOT_TOP;
		writeCmd(DisplayST7735::MEMORY_DATA_ACCESS_CONTROL);
		writeNData(&MemoryAccessControl, 1);
	}
}

void DisplayST7735::setPixelFormat(uint8_t pf) {
	if (this->PixelFormat != pf) {
		this->PixelFormat = pf;
		writeCmd(DisplayST7735::INTERFACE_PIXEL_FORMAT);
		writeNData(&pf, 1);
	}
}


bool DisplayST7735::writeCmd(uint8_t c) {
	return writeN(0, &c, sizeof(c));
}

bool DisplayST7735::writeNData(const uint8_t* data, int nbytes) {
	writeCmd(DisplayST7735::MEMORY_WRITE);
	return writeN(1, data, nbytes);
}

bool DisplayST7735::writeN(char dc, const uint8_t* data, int nbytes) {
#if !defined VIRTUAL_DEVICE
	if (dc == 1) { //dc 1=data 0 = control
		HAL_GPIO_WritePin(HardwareConfig::getDataCmd().Port, HardwareConfig::getDataCmd().Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(HardwareConfig::getDataCmd().Port, HardwareConfig::getDataCmd().Pin, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(HardwareConfig::getCS().Port, HardwareConfig::getCS().Pin, GPIO_PIN_RESET);
	if (HAL_OK != HAL_SPI_Transmit(HardwareConfig::getSPI(), const_cast<uint8_t*>(data), nbytes, 1000)) {
		return false;
	}
	HAL_GPIO_WritePin(HardwareConfig::getCS().Port, HardwareConfig::getCS().Pin, GPIO_PIN_SET);
#endif
	return true;
}

bool DisplayST7735::write16Data(const uint16_t& data) {
	uint8_t buf[2];
	buf[0] = data >> 8;
	buf[1] = data & 0xFF;
	return writeN(1, &buf[0], sizeof(buf));
}

void DisplayST7735::swap() {
	setMemoryAccessControl();
	FB->swap();
}

void DisplayST7735::drawImage(int16_t x, int16_t y, const DCImage& dcImage) {
	FB->drawImage(x, y, dcImage);
}

bool DisplayST7735::drawPixel(uint16_t x0, uint16_t y0, const RGBColor& color) {
	return FB->drawPixel(x0, y0, color);
}

void DisplayST7735::setBackLightOn(bool on) {
#if !defined VIRTUAL_DEVICE

	if (on)
		HAL_GPIO_WritePin(HardwareConfig::getBackLit().Port, HardwareConfig::getBackLit().Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(HardwareConfig::getBackLit().Port, HardwareConfig::getBackLit().Pin, GPIO_PIN_RESET);
#endif
}

void DisplayST7735::reset() {
#if !defined VIRTUAL_DEVICE

	//clear chip select
	HAL_GPIO_WritePin(HardwareConfig::getCS().Port, HardwareConfig::getCS().Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HardwareConfig::getReset().Port, HardwareConfig::getReset().Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(HardwareConfig::getReset().Port, HardwareConfig::getReset().Pin, GPIO_PIN_RESET);
	setBackLightOn(false);
	HAL_Delay(20);
	HAL_GPIO_WritePin(HardwareConfig::getReset().Port, HardwareConfig::getReset().Pin, GPIO_PIN_SET);
	setBackLightOn(true);
	HAL_Delay(10);

	for (const sCmdBuf* cmd = initializers; cmd->command; cmd++) {
		writeCmd(cmd->command);
		if (cmd->len)
			writeNData(cmd->data, cmd->len);
		if (cmd->delay)
			HAL_Delay(cmd->delay);
	}
#endif
}

ErrorType DisplayST7735::onInit() {
	ErrorType et;
	setBackLightOn(true);

	reset();
	//ensure memory access control format
	setMemoryAccessControl();

	fillScreen(RGBColor::BLACK);
	swap();
	return et;
}

void DisplayST7735::fillScreen(const RGBColor& color) {
	fillRec(0, 0, getWidth(), getHeight(), color);
}

// Draw a filled rectangle at the given coordinates with the given width, height, and color.
// Input: x     horizontal position of the top left corner of the rectangle, columns from the left edge
//        y     vertical position of the top left corner of the rectangle, rows from the top edge
//        w     horizontal width of the rectangle
//        h     vertical height of the rectangle
//        color appropriated packed color, which can be produced by PackColor::create()
// Output: none
void DisplayST7735::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
	//PackedColor pc = PackedColor::create(PixelFormat, color);

	if ((x >= getWidth()) || (y >= getHeight()))
		return;
	if ((x + w - 1) >= getWidth())
		w = getWidth() - x;
	if ((y + h - 1) >= getHeight())
		h = getHeight() - y;

	getFrameBuffer()->fillRec(x, y, w, h, color);
}

void DisplayST7735::drawRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
	drawHorizontalLine(x, y, w, color);
	drawVerticalLine(x, y, h, color);
	drawHorizontalLine(x, y + h >= getHeight() ? getHeight() - 1 : y + h, w, color);
	drawVerticalLine(x + w, y, h, color);
}


// Input: x         horizontal position of the top left corner of the character, columns from the left edge
//        y         vertical position of the top left corner of the character, rows from the top edge
//        c         character to be printed
//        textColor 16-bit color of the character
//        bgColor   16-bit color of the background
//        size      number of pixels per character pixel (e.g. size==2 prints each pixel of font as 2x2 square)
// Output: none
void DisplayST7735::drawCharAtPosition(int16_t x, int16_t y, char c, const RGBColor& textColor, const RGBColor& bgColor,
	uint8_t size) {
	uint8_t line; // vertical column of pixels of character in font
	int32_t i, j;
	if ((x >= getWidth()) || // Clip right
		(y >= getHeight()) || // Clip bottom
		((x + 5 * size - 1) < 0) || // Clip left
		((y + 8 * size - 1) < 0))   // Clip top
		return;

	for (i = 0; i < CurrentFont->FontWidth; i++) {
		if (i == CurrentFont->FontWidth - 1)
			line = 0x0;
		else
			line = getFont()->data[(c * CurrentFont->CharBytes) + i];
		for (j = 0; j < 8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
					drawPixel(x + i, y + j, textColor);
				else {  // big size
					fillRec(x + (i * size), y + (j * size), size, size, textColor);
				}
			}
			else if (bgColor != textColor) {
				if (size == 1) // default size
					drawPixel(x + i, y + j, bgColor);
				else {  // big size
					fillRec(x + i * size, y + j * size, size, size, bgColor);
				}
			}
			line >>= 1;
		}
	}
}

uint32_t DisplayST7735::drawStringOnLine(uint8_t line, const char* msg) {
	return drawString(0, getFont()->FontHeight * line, msg);
}

uint32_t DisplayST7735::drawString(uint16_t x, uint16_t y, const char* pt) {
	return drawString(x, y, pt, CurrentTextColor);
}

uint32_t DisplayST7735::drawString(uint16_t x, uint16_t y, const char* pt, const RGBColor& textColor) {
	return drawString(x, y, pt, textColor, CurrentBGColor, 1, false);
}

uint32_t DisplayST7735::drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor,
	const RGBColor& backGroundColor, uint8_t size, bool lineWrap) {
	uint16_t currentX = xPos;
	uint16_t currentY = yPos;
	const char* orig = pt;

	while (*pt) {
		if ((currentX > getWidth() && !lineWrap) || currentY > getHeight()) {
			return pt - orig;
		}
		else if (currentX > getWidth() && lineWrap) {
			currentX = 0;
			currentY += CurrentFont->FontHeight * size;
			drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
			currentX += CurrentFont->FontWidth;
		}
		else if (*pt == '\n' || *pt == '\r') {
			currentY += CurrentFont->FontHeight * size;
			currentX = 0;
		}
		else {
			drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
			currentX += CurrentFont->FontWidth * size;
		}
		pt++;
	}
	return (pt - orig);  // number of characters printed
}

uint32_t DisplayST7735::drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor,
	const RGBColor& backGroundColor, uint8_t size, bool lineWrap, uint8_t charsToRender) {
	uint16_t currentX = xPos;
	uint16_t currentY = yPos;
	const char* orig = pt;

	while (charsToRender-- && *pt) {
		if ((currentX > getWidth() && !lineWrap) || currentY > getHeight()) {
			return pt - orig;
		}
		else if (currentX > getWidth() && lineWrap) {
			currentX = 0;
			currentY += CurrentFont->FontHeight * size;
			drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
			currentX += CurrentFont->FontWidth;
		}
		else if (*pt == '\n' || *pt == '\r') {
			currentY += CurrentFont->FontHeight * size;
			currentX = 0;
		}
		else {
			drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
			currentX += CurrentFont->FontWidth * size;
		}
		pt++;
	}
	return (pt - orig);  // number of characters printed
}

void DisplayST7735::drawVerticalLine(int16_t x, int16_t y, int16_t h) {
	drawVerticalLine(x, y, h, CurrentTextColor);
}

// Draw a vertical line at the given coordinates with the given height and color.
// A vertical line is parallel to the longer side of the rectangular display
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        h     vertical height of the line
//		color	RGB color of line
void DisplayST7735::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) {
	// safety
	if ((x >= getWidth()) || (y >= getHeight()))
		return;
	if ((y + h - 1) >= getHeight())
		h = getHeight() - y;
	getFrameBuffer()->drawVerticalLine(x, y, h, color);
}

void DisplayST7735::drawHorizontalLine(int16_t x, int16_t y, int16_t w) {
	return drawHorizontalLine(x, y, w, CurrentTextColor);
}

// Draw a horizontal line at the given coordinates with the given width and color.
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        w     horizontal width of the line
//		Color is the RGBColor
void DisplayST7735::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
	//safey
	if ((x >= getWidth()) || (y >= getHeight()))
		return;
	if ((x + w - 1) >= getWidth())
		w = getWidth() - x;

	getFrameBuffer()->drawHorizontalLine(x, y, w, color);
}

//////////////////////////////////////////////////////////////////////
	// not using buffer just write directly to SPI
bool DrawBufferNoBuffer::drawPixel(uint16_t x0, uint16_t y0, const RGBColor& color) {
	PackedColor pc = PackedColor::create(PixelFormat, color);
	setAddrWindow(x0, y0, x0, y0);
	return writeNData(pc.getPackedColorData(), pc.getSize());
}

void DrawBufferNoBuffer::drawImage(int16_t x, int16_t y, const DCImage& dc) {
	setAddrWindow(0, 0, dc.width, dc.height);
	writeNData((const uint8_t*)&dc.pixel_data[0], dc.height * dc.width * dc.bytes_per_pixel);
}

void DrawBufferNoBuffer::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
	PackedColor pc = PackedColor::create(PixelFormat, color);

	uint16_t pcolor = *((uint16_t*)(pc.getPackedColorData()));
	uint16_t pixelCount = w * h;
	uint16_t maxAtOnce =
		pixelCount > (RowsForDrawBuffer * Display->getWidth()) ?
		(RowsForDrawBuffer * Display->getWidth()) : pixelCount;
	for (uint16_t i = 0; i < maxAtOnce; ++i) {
		SPIBuffer[i] = pcolor;
	}

	setAddrWindow(x, y, w, h);
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
	PackedColor pc = PackedColor::create(PixelFormat, color);

	uint16_t pcolor = *((uint16_t*)(pc.getPackedColorData()));
	uint16_t pixelCount = h;
	uint16_t maxAtOnce =
		pixelCount > (RowsForDrawBuffer * Display->getWidth()) ?
		(RowsForDrawBuffer * Display->getWidth()) : pixelCount;
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
	PackedColor pc = PackedColor::create(PixelFormat, color);

	uint16_t pcolor = *((uint16_t*)(pc.getPackedColorData()));
	for (uint16_t i = 0; i < w; ++i) {
		SPIBuffer[i] = pcolor;
	}

	setAddrWindow(x, y, x + w - 1, y);
	writeNData((uint8_t*)&SPIBuffer[0], w * sizeof(uint16_t));
}