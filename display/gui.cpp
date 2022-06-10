#include "gui.h"
#include "framebuf.h"
#include <cstdint>

using namespace cmdc0de;

extern uint32_t HAL_GetTick(void);


void GUIListItemData::setShouldScroll()
{
   if (shouldScroll())
   {
      resetScrollable();
   }
   else
   {
      Scrollable = 0;
   }
}

bool GUIListItemData::shouldScroll()
{
   if (strlen(text) * 6 > 120)
   {
      return true;
   }
   return false;
}

GUITickerData::GUITickerData(const char* txt, uint8_t X, uint8_t Y,
   uint8_t W, uint8_t H) :
   text(txt), x(X), y(Y), w(W), h(H), BorderSize(1), FontScalar(1), bg(
      RGBColor::BLACK), TextColor(RGBColor::WHITE), startTick(0)
{}

void GUI::drawTicker(GUITickerData* dt) const
{
   uint8_t maxlen = (dt->w - dt->BorderSize * 2) / font->FontWidth, len = 0;
   int shift = 0;
   while (*(dt->text + len) != 0)
   {
      len++;
   }
   if (dt->startTick == 0)
   {
      dt->startTick = HAL_GetTick();
   }
   shift = ((HAL_GetTick() - dt->startTick) / GUI_TickerSpeed) - maxlen / 2; //start delay

   if (shift > len - maxlen)
   {
      if (shift > len - maxlen + GUI_TickerEndDelay)
      {
         shift = 0;
         dt->startTick = HAL_GetTick();
      }
      else
      {
         shift = len - maxlen;
      }
   }
   if (shift < 0)
   {
      shift = 0;
   }
   drawString(dt->x, dt->y, dt->text + shift, dt->TextColor, dt->bg, dt->FontScalar, false);
}

const char* GUIListItemData::getScrollOffset()
{
   uint16_t offSet = 0;
   if (Scrollable)
   {
      if (LastScrollTime == 0)
      {
         LastScrollTime = HAL_GetTick();
      }
      if (HAL_GetTick() - LastScrollTime > TimeBetweenScroll)
      {
         LastScrollTime = HAL_GetTick();
         LastScrollPosition++;
         uint32_t l = strlen(text);
         //char b[10];
         //sprintf(&b[0],"%d",l);
         if (LastScrollPosition >= l)
         {
            LastScrollPosition = 0;
         }
      }
      offSet = LastScrollPosition;
   }
   return text + offSet;
}
void cmdc0de::GUI::drawImage(uint16_t x, int16_t y, const DCImage& image)
{
    frameBuf->drawImage(x, y, image);
}
void GUI::drawCharAtPosition(int16_t x, int16_t y, char c, const RGBColor& textColor, const RGBColor& bgColor, uint8_t size) const
{
   uint8_t line; // vertical column of pixels of character in font
   int32_t i, j;
   if ((x >= frameBuf->getWidth()) || // Clip right
      (y >= frameBuf->getHeight()) || // Clip bottom
      ((x + 5 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
   {
      return;
   }

   for (i = 0; i < font->FontWidth; i++)
   {
      if (i == font->FontWidth - 1)
      {
         line = 0x0;
      }
      else
      {
         line = getFont()->data[(c * font->CharBytes) + i];
      }
      for (j = 0; j < 8; j++)
      {
         if (line & 0x1)
         {
            if (size == 1) // default size
            {
               frameBuf->drawPixel(x + i, y + j, textColor);
            }
            else
            {  // big size
              frameBuf->fillRec(x + (i * size), y + (j * size), size, size, textColor);
            }
         }
         else if (bgColor != textColor)
         {
            if (size == 1) // default size
            {
               frameBuf->drawPixel(x + i, y + j, bgColor);
            }
            else // big size
            {
              frameBuf->fillRec(x + i * size, y + j * size, size, size, bgColor);
            }
         }
         line >>= 1;
      }
   }
}

uint32_t GUI::drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor, const RGBColor& backGroundColor, uint8_t size, bool lineWrap) const
{
   uint16_t currentX = xPos;
   uint16_t currentY = yPos;
   const char* orig = pt;

   while (*pt)
   {
      if ((currentX > frameBuf->getWidth() && !lineWrap) || currentY > frameBuf->getHeight())
      {
         return pt - orig;
      }
      else if (currentX > frameBuf->getWidth() && lineWrap)
      {
         currentX = 0;
         currentY += font->FontHeight * size;
         drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
         currentX += font->FontWidth;
      }
      else if (*pt == '\n' || *pt == '\r')
      {
         currentY += font->FontHeight * size;
         currentX = 0;
      }
      else
      {
         drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
         currentX += font->FontWidth * size;
      }
      pt++;
   }
   return (pt - orig);  // number of characters printed
}

uint32_t GUI::drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor, const RGBColor& backGroundColor, uint8_t size, bool lineWrap, uint8_t charsToRender) const
{
   uint16_t currentX = xPos;
   uint16_t currentY = yPos;
   const char* orig = pt;

   while (charsToRender-- && *pt)
   {
      if ((currentX > frameBuf->getWidth() && !lineWrap) || currentY > frameBuf->getHeight())
      {
         return pt - orig;
      }
      else if (currentX > frameBuf->getWidth() && lineWrap)
      {
         currentX = 0;
         currentY += font->FontHeight * size;
         drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
         currentX += font->FontWidth;
      }
      else if (*pt == '\n' || *pt == '\r')
      {
         currentY += font->FontHeight * size;
         currentX = 0;
      }
      else
      {
         drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
         currentX += font->FontWidth * size;
      }
      pt++;
   }
   return (pt - orig);  // number of characters printed
}

uint8_t GUI::drawList(GUIListData* gui_CurList) const
{
   if (gui_CurList == 0)
      return 0;

   frameBuf->fillRec(gui_CurList->x, gui_CurList->y, gui_CurList->w, gui_CurList->h, RGBColor::BLACK);
   frameBuf->drawRec(gui_CurList->x, gui_CurList->y, gui_CurList->w - 1, gui_CurList->h, RGBColor::BLUE);

   uint8_t ry = gui_CurList->y + 2;
   uint8_t rx = gui_CurList->x + 4;
   if (gui_CurList->header != 0)
   {
      drawString(rx, ry, gui_CurList->header, RGBColor::WHITE, RGBColor::BLACK, 1, false);
      ry += font->FontHeight;
   }

   uint8_t maxC = ((gui_CurList->h - 3) / font->FontHeight) - (gui_CurList->header != 0);

   uint16_t i;
   if (maxC >= gui_CurList->ItemsCount)
   {
      for (i = 0; i < gui_CurList->ItemsCount; i++)
      {
         if (i != gui_CurList->selectedItem)
         {
            drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].text, RGBColor::WHITE, RGBColor::BLACK, 1, false);
         }
         else
         {
            drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE, 1, false);
         }
      }
   }
   else
   {
      if (gui_CurList->ItemsCount - 1 - gui_CurList->selectedItem < maxC / 2)
      {
         for (i = gui_CurList->ItemsCount - maxC;
            i < gui_CurList->ItemsCount; i++)
         {
            if (i != gui_CurList->selectedItem)
            {
               drawString(rx, ry + (i - gui_CurList->ItemsCount + maxC) * font->FontHeight, gui_CurList->items[i].text, RGBColor::WHITE, RGBColor::BLACK, 1, false);
            }
            else
            {
               drawString(rx, ry + (i - gui_CurList->ItemsCount + maxC) * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE, 1, false);
            }
         }
      }
      else if (gui_CurList->selectedItem < maxC / 2)
      {
         for (i = 0; i < maxC; i++)
         {
            if (i != gui_CurList->selectedItem)
            {
               drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].text, RGBColor::WHITE, RGBColor::BLACK, 1, false);
            }
            else
            {
               drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE, 1, false);
            }
         }
      }
      else
      {
         for (i = gui_CurList->selectedItem - maxC / 2; i < gui_CurList->selectedItem - maxC / 2 + maxC; i++)
         {
            if (i != gui_CurList->selectedItem)
            {
               drawString(rx, ry + (i - gui_CurList->selectedItem + maxC / 2) * font->FontHeight, gui_CurList->items[i].text, RGBColor::WHITE, RGBColor::BLACK, 1, false);
            }
            else
            {
               drawString(rx, ry + (i - gui_CurList->selectedItem + maxC / 2) * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE, 1, false);
            }
         }
      }
   }
   uint8_t sli_h = gui_CurList->h / gui_CurList->ItemsCount;
   if (sli_h < 14)
      sli_h = 14;
   frameBuf->drawHorizontalLine(gui_CurList->x + 2, ry - 2, gui_CurList->x + gui_CurList->w - 2, RGBColor::WHITE);
   return 0;
}

