#include "DrawBuffer2D16BitPerPixel1Buffer.h"

namespace cmdc0de {

   bool DrawBuffer2D16BitPerPixel1Buffer::drawPixel(uint16_t x, uint16_t y, RGBColor color)
   {
      if (x >= Width || y >= Height)
      {
         return false;
      }
      SPIBuffer[(y * Width) + x] = color;
      return true;
   }

   void DrawBuffer2D16BitPerPixel1Buffer::drawImage(int16_t xOffset, int16_t yOffset, const DCImage& dc)
   {
#if 0
      setAddrWindow(0, 0, dc.width, dc.height);
      writeNData((const uint8_t*)&dc.pixel_data[0], dc.height * dc.width * dc.bytes_per_pixel);
#else
      auto source = (uint16_t*)dc.pixel_data;
      for (auto yImg = 0; yImg < dc.height; ++yImg)
      {
         for (int xImg = 0; xImg < dc.width; ++xImg)
         {
            auto pixelColor = source[yImg * dc.width + xImg];
            SPIBuffer[(yOffset + yImg) * Width + xOffset + xImg] = pixelColor;
         }
         //memcpy(&SPIBuffer[(yOffset + yImg) * Width + xOffset], &dc.pixel_data[yImg * dc.width], dc.width * dc.bytes_per_pixel);
      }
      //for (auto y = 0; y < dc.height; ++y)
      //{
      //   for (int x = 0; x < dc.width; ++x)
      //   {
      //      uint16_t c = (uint16_t)dc.pixel_data[(y * dc.width) + x + 1] << 8 | dc.pixel_data[(y * dc.width) + x];
      //      //auto red = (t[(y * dc.width) + x] & 0b1111100000000000) >> 11;
      //      //auto green = (t[(y * dc.width) + x] & 0b0000011111100000) >> 5;
      //      //auto blue = t[(y * dc.width) + x] & 0b0000000000011111;
      //      //RGBColor c(red, green, blue);
      //      //uint8_t* packedData = PackedColor::create(PixelFormat, c).getPackedColorData();

      //      auto bufferX = x + xOffset;
      //      auto bufferY = (y + yOffset) * Width;
      //      //SPIBuffer[bufferY + bufferX] = *((uint16_t *)packedData);
      //      SPIBuffer[bufferY + bufferX] = calcLCDColor(c);
      //   }
      //}
#endif
   }

   void DrawBuffer2D16BitPerPixel1Buffer::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color)
   {
      for (int i = y; i < (h + y); ++i)
      {
         //OPTIMIZE THIS BY MAKING A SET RANGE IN BITARRAY
         uint32_t offset = i * Width;
         for (int j = 0; j < w; ++j)
         {
            SPIBuffer[offset + x + j] = color;
         }
      }
   }

   void DrawBuffer2D16BitPerPixel1Buffer::drawVerticalLine(int16_t x, int16_t y, int16_t h, RGBColor color)
   {
      for (int i = y; i < (h + y); ++i)
      {
         SPIBuffer[i * Width + x] = color;
      }
   }

   void DrawBuffer2D16BitPerPixel1Buffer::drawHorizontalLine(int16_t x, int16_t y, int16_t w, RGBColor color)
   {
      uint32_t offset = y * Width;
      for (int i = x; i < (x + w); ++i)
      {
         SPIBuffer[offset + i] = color;
      }
   }

   void DrawBuffer2D16BitPerPixel1Buffer::swap()
   {
      if (/*anychange*/ 1)
      {
         /*setAddrWindow(0, 0, Width, Display->getHeight());
         writeNData((uint8_t*)&SPIBuffer[0], BufferSize);*/
         //Display->update();
      }
   }
}