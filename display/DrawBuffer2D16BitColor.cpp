#include "DrawBuffer2D16BitColor.h"

namespace cmdc0de {
   bool DrawBuffer2D16BitColor::drawPixel(uint16_t x, uint16_t y, RGBColor color)
   {
      //uint8_t c = deresColor(color);
      //BackBuffer.setValueAsByte((y * Width) + x, c);
      //DrawBlocksChanged.setValueAsByte(y / RowsForDrawBuffer, 1);
      //return true;

      if (x > Width || y > Height) return false;
      SPIBuffer[(y * Width) + x] = color;
      return true;
   }

   void DrawBuffer2D16BitColor::drawImage(int16_t x, int16_t y, const DCImage& dc)
   {
      //setAddrWindow(0, 0, dc.width - 1, dc.height - 1);auto source = (uint16_t*)dc.pixel_data;
      auto source = (uint16_t*)dc.pixel_data;
      for (auto yImg = 0; yImg < dc.height; ++yImg)
      {
         for (int xImg = 0; xImg < dc.width; ++xImg)
         {
            auto pixelColor = source[yImg * dc.width + xImg];
            SPIBuffer[(y + yImg) * Width + x + xImg] = pixelColor;
         }
      }
      swap();
      DrawBlocksChanged.clear();
   }

   void DrawBuffer2D16BitColor::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color)
   {
      uint8_t c = deresColor(color);
      for (int i = y; i < (h + y); ++i)
      {
         //OPTIMIZE THIS BY MAKING A SET RANGE IN BITARRAY
         uint32_t offset = i * Width;
         for (int j = 0; j < w; ++j)
         {
            BackBuffer.setValueAsByte(offset + x + j, c);
         }
         DrawBlocksChanged.setValueAsByte(i / RowsForDrawBuffer, 1);
      }
   }

   void DrawBuffer2D16BitColor::drawVerticalLine(int16_t x, int16_t y, int16_t h, RGBColor color)
   {
      uint8_t c = deresColor(color);
      for (int i = y; i < (h + y); ++i)
      {
         BackBuffer.setValueAsByte(i * Width + x, c);
         DrawBlocksChanged.setValueAsByte(i / RowsForDrawBuffer, 1);
      }
   }

   void DrawBuffer2D16BitColor::drawHorizontalLine(int16_t x, int16_t y, int16_t w, RGBColor color)
   {
      uint8_t c = deresColor(color);
      uint32_t offset = y * Width;
      for (int i = x; i < (x + w); ++i)
      {
         BackBuffer.setValueAsByte(offset + i, c);
      }
      DrawBlocksChanged.setValueAsByte(y / RowsForDrawBuffer, 1);
   }

   //////
   // first check to see if we changed anything in the draw block, if not skip it
   // if we did change something convert from our short hand notation to something the LCD will understand
   //	then send to LCD
   void DrawBuffer2D16BitColor::swap()
   {
      for (int h = 0; h < Height; h++)
      {
         if ((DrawBlocksChanged.getValueAsByte(h / RowsForDrawBuffer)) != 0)
         {
            /*for (int w = 0; w < Width; w++)
            {
               uint32_t SPIY = h % RowsForDrawBuffer;
               SPIBuffer[(SPIY * Width) + w] = BackBuffer.getValueAsByte((h * Width) + w);
            }*/
            /*if (h != 0 && (h % RowsForDrawBuffer == (RowsForDrawBuffer - 1))) {
              setAddrWindow(0, h - (RowsForDrawBuffer - 1), Width, h);
              writeNData((uint8_t*)&SPIBuffer[0], Width * RowsForDrawBuffer * sizeof(uint16_t));
            }*/
         }
      }
      DrawBlocksChanged.clear();
   }
}