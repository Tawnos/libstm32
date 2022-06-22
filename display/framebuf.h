#pragma once

#include <rgbcolor.h>
#include <cassert>
#include <stdint.h>
#include <art/images.h>
#include <common.h>

namespace cmdc0de
{
   class DisplayDevice;
   /// enumeration of pixel formats for interface pixel command
   enum class PixelFormat
   {
      TwelveBit = 0b011,
      SixteenBit = 0b101,
      EighteenBit = 0b110
   };

   /// Used to convert from the RGB Color class to something the driver chip understands
   class PackedColor
   {
   public:
      PackedColor() : Color{ 0 }, SizeInBytes(2) {}
      constexpr uint8_t* getPackedColorData() { return &Color[0]; }
      constexpr uint8_t getSize() { return SizeInBytes; }

   public:
      static PackedColor create(PixelFormat pixelFormat, RGBColor c)
      {
         PackedColor pc;
         switch (pixelFormat)
         {
         case PixelFormat::TwelveBit:
            pc.SizeInBytes = 2;
            break;
         case PixelFormat::SixteenBit:
         {
            uint16_t tmp;
            tmp = (c.getR() & 0b11111) << 11;
            tmp |= (c.getG() & 0b111111) << 5;
            tmp |= (c.getB() & 0b11111);
            pc.Color[0] = tmp >> 8;
            pc.Color[1] = tmp & 0xFF;
            pc.SizeInBytes = 2;
         }
         break;
         case PixelFormat::EighteenBit:
            pc.Color[0] = c.getR() << 2;
            pc.Color[1] = c.getG() << 2;
            pc.Color[2] = c.getB() << 2;
            pc.SizeInBytes = 3;
            break;
         default:
            assert(false);
         }
         return pc;
      }

   private:
      uint8_t Color[3];
      uint8_t SizeInBytes;
   };

   /*
    * Allows us to have different frame buffer types to make optimal use of memory
    * Current implemented (see below)
    * pass though
    * 2d buffer
    * 3d buffer
    */
   class FrameBuf
   {
   public:
      virtual void swap() = 0;

      virtual void drawImage(int16_t x, int16_t y, const DCImage& dcImage) = 0;
      virtual bool drawPixel(uint16_t x0, uint16_t y0, RGBColor color) = 0;
      virtual void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color) = 0;
      virtual void drawRec(int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color);
      virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, RGBColor color) = 0;
      virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, RGBColor color) = 0;

      constexpr uint8_t getWidth() const
      {
         return Width;
      }


      constexpr uint8_t getHeight() const
      {
         return Height;
      }

   protected:
      FrameBuf(uint16_t bufferSize, uint16_t* SPIBuffer, uint8_t w, uint8_t h, PixelFormat pf)
         : SPIBuffer(SPIBuffer),
         BufferSize(bufferSize),
         PixelFormat(pf),
         Width(w),
         Height(h)
      {}

      uint16_t* SPIBuffer{ nullptr };
      uint16_t BufferSize{ 0 };
      const PixelFormat PixelFormat{ PixelFormat::SixteenBit };
      uint8_t Width;
      uint8_t Height;

      friend class DisplayDevice;
   };


}
