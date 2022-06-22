#pragma once

#include <rgbcolor.h>
#include <cassert>
#include <stdint.h>
#include "display_device.h"
#include "framebuf.h"
#include <rgbcolor.h>

namespace cmdc0de
{
   /*
    * @author cmdc0de
    * @date 6/2/17
    *
    * 2 byte per pixel for backbuffer
    * 5 bits Red, 6 Bits Green, 5 bits blue -
    * for a 128x160 lcd you'll need 40,960 bytes for the buffer
    */
   class DrawBuffer2D16BitPerPixel1Buffer : public FrameBuf
   {
   public:
      enum COLOR
      { // 2/2/2
         BLACK = 0,
         RED_MASK = 0x30,
         GREEN_MASK = 0xC,
         BLUE_MASK = 0x3,
         WHITE = 0xFFFF,
         BITS_PER_PIXEL = 5
      };

   public:
      DrawBuffer2D16BitPerPixel1Buffer(uint16_t bufferSize, uint16_t* spiBuffer, uint8_t w, uint8_t h)
         : FrameBuf(bufferSize, spiBuffer, w, h, PixelFormat::SixteenBit)
      {}
      virtual ~DrawBuffer2D16BitPerPixel1Buffer() = default;

      virtual void drawImage(int16_t x, int16_t y, const DCImage& dc) override;
      virtual bool drawPixel(uint16_t x, uint16_t y, RGBColor color) override;
      virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, RGBColor color) override;
      virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, RGBColor color) override;
      virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color) override;
      virtual void swap() override;
   };
}