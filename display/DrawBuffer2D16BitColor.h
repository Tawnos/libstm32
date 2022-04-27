#pragma once
#include "display_device.h"
#include "framebuf.h"
#include <rgbcolor.h>
#include <utility/bitarray.h>

namespace cmdc0de {
  /*
   * @author cmdc0de
   * @date 6/2/17
   *
   * 1 byte per pixel for backbuffer
   * 3 bits Red, 2 Bits Green, 3 bits blue - color passed in is deresed from 565 to 323
   * On each pixel write we track if we changed any pixel in rowsForDrawBuffer,
   * if not on swap we skip those rows, otherwise write 2 bytes (565) to drawBuffer
   * for the width of the screen for 'rowsForDrawBuffer' rows. flush to SPI bus then repeat until
   * entire screen is rendered
   */
  class DrawBuffer2D16BitColor : public FrameBuf
  {
  public:
    enum COLOR
    { // 2/2/2
      BLACK = 0,
      RED_MASK = 0x30,
      GREEN_MASK = 0xC,
      BLUE_MASK = 0x3,
      WHITE = 0x3F,
      BITS_PER_PIXEL = 2
    };

  public:
    DrawBuffer2D16BitColor(
      DisplayDevice* d,
      uint8_t* backBuffer,
      uint16_t* spiBuffer,
      uint8_t rowsForDrawBuffer,
      uint8_t* drawBlocksBuffer
    ) : FrameBuf(d, spiBuffer, d->getWidth()* d->getHeight(), d->getWidth(), d->getHeight(), PixelFormat::SixteenBit),
      BackBuffer{ backBuffer, d->getWidth() * d->getHeight(), 6 },
      RowsForDrawBuffer(rowsForDrawBuffer),
      DrawBlocksChanged{ drawBlocksBuffer, d->getHeight() / rowsForDrawBuffer, 1 } {}

    virtual ~DrawBuffer2D16BitColor() = default;
    virtual void drawImage(int16_t x, int16_t y, const DCImage& dc) override;
    virtual bool drawPixel(uint16_t x, uint16_t y, const RGBColor& color) override;
    virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor& color) override;
    virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) override;
    virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor& color) override;
    virtual void swap() override;

  protected:
    uint16_t calcLCDColor(uint8_t packedColor);
    uint8_t deresColor(const RGBColor& color);

  private:
    BitArray BackBuffer{ nullptr,0,0 };
    uint8_t RowsForDrawBuffer = 0;
    BitArray DrawBlocksChanged{ nullptr,0,0 };
  };
}