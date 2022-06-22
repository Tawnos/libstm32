#include "framebuf.h"
namespace cmdc0de {

   void FrameBuf::drawRec(int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color)
   {
      auto topRow = y >= Height
         ? Height - 1
         : y < 0
            ? 0
            : y;
      auto leftCol =
         x >= Width
         ? Width - 1
         : x + w < 0
            ? 0
            : x;
      drawHorizontalLine(leftCol, topRow, w, color);
      drawVerticalLine(x, y, h, color);
      auto bottomRow = 
         y + h >= Height
            ? Height - 1
            : y + h < 0 
               ? 0 
               : y + h;
      auto rightCol = 
         x + w >= Width
         ? Width - 1
         : x + w < 0
            ? 0
            : x + w;
      drawHorizontalLine(x, bottomRow, w, color);
      drawVerticalLine(rightCol, y, h, color);
   }



}