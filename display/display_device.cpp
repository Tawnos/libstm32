#include "display_device.h"
#include "framebuf.h"
#include "../config.h"
#include "../logger.h"
#include "assert.h"
#include "framebuf.h"
#include <rgbcolor.h>
#include <string.h>
//#include <diag/Trace.h>

namespace cmdc0de {

  void FrameBuf::drawRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
    drawHorizontalLine(x, y, w, color);
    drawVerticalLine(x, y, h, color);
    auto bottomRow = (y + h) >= Display->getHeight() ? (Display->getHeight() - 1) : y + h;
    auto rightCol = (x + w) >= Display->getWidth() ? (Display->getWidth() - 1) : x + w;
    drawHorizontalLine(x, bottomRow, w, color);
    drawVerticalLine(rightCol, y, h, color);
  }


  bool DrawBuffer2D16BitColor16BitPerPixel1Buffer::drawPixel(uint16_t x, uint16_t y, const RGBColor& color) {
    SPIBuffer[(y * Display->getWidth()) + x] = calcLCDColor(color);
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
        auto red = (t[(y * dc.height) + x] & 0b1111100000000000) >> 11;
        auto green = (t[(y * dc.height) + x] & 0b0000011111100000) >> 5;
        auto blue = t[(y * dc.height) + x] & 0b0000000000011111;
        RGBColor c(red, green, blue);

        uint8_t* packedData = PackedColor::create(PixelFormat, c).getPackedColorData();
        uint16_t* pcd = (uint16_t*)packedData;
        SPIBuffer[((y + y1) * Display->getWidth()) + (x + x1)] = (*pcd);
      }
    }
    Display->update();
#endif
  }

  void DrawBuffer2D16BitColor16BitPerPixel1Buffer::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) {
    uint16_t c = calcLCDColor(color);
    for (int i = y; i < (h + y); ++i) {
      //OPTIMIZE THIS BY MAKING A SET RANGE IN BITARRAY
      uint32_t offset = i * Display->getWidth();
      for (int j = 0; j < w; ++j) {
        SPIBuffer[offset + x + j] = c;
      }
    }
  }

  void DrawBuffer2D16BitColor16BitPerPixel1Buffer::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) {
    uint16_t c = calcLCDColor(color);
    for (int i = y; i < (h + y); ++i) {
      SPIBuffer[i * Display->getWidth() + x] = c;
    }
  }

  void DrawBuffer2D16BitColor16BitPerPixel1Buffer::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
    uint16_t c = calcLCDColor(color);
    uint32_t offset = y * Display->getWidth();
    for (int i = x; i < (x + w); ++i) {
      SPIBuffer[offset + i] = c;
    }
  }

  void DrawBuffer2D16BitColor16BitPerPixel1Buffer::swap() {
    if (/*anychange*/ 1) {
      /*setAddrWindow(0, 0, Display->getWidth(), Display->getHeight());
      writeNData((uint8_t*)&SPIBuffer[0], BufferSize);*/
      Display->update();
    }
  }

  uint16_t DrawBuffer2D16BitColor16BitPerPixel1Buffer::calcLCDColor(const RGBColor& color) {
    uint32_t rc = color.getR() / 8; //keep it in 5 bits
    uint32_t gc = color.getG() / 4; //keep it in 6 bits
    uint32_t bc = color.getB() / 8; //keep it in 5 bits
    RGBColor lcdColor(rc, gc, bc);
    uint8_t* packedData = PackedColor::create(PixelFormat, lcdColor).getPackedColorData();
    uint16_t* pcd = (uint16_t*)packedData;
    return *pcd;
  }
}