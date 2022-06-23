#include "gui.h"
#include "framebuf.h"

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
   drawString(dt->x, dt->y, dt->text + shift);
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
void GUI::drawCharAtPosition(int16_t x, int16_t y, char c, RGBColor textColor, RGBColor bgColor) const
{
   if (x >= frameBuf->getWidth() 
    || y >= frameBuf->getHeight())
      //|| x + font->FontWidth >= frameBuf->getWidth() 
      //|| y + font->FontHeight >= frameBuf->getHeight())
   {
      return;
   }


   uint8_t maxCol = x + font->FontWidth < frameBuf->getWidth() ? font->FontWidth : frameBuf->getWidth() - x;
   uint8_t maxRow = y + font->FontHeight < frameBuf->getHeight() ? font->FontHeight : frameBuf->getHeight() - y;
   const uint16_t* fontData;

   if (font->ByteOrder == FontByteOrder::Row)
   {
      fontData = &font->data[(c - ' ') * (font->FontWidth-1)];
      for (uint8_t i = 0; i < maxCol; i++)
      {
         auto pixelCol = fontData[i];

         for (auto j = 0; j < maxRow; j++)
         {
            auto color = (pixelCol & 0x1) ? textColor : bgColor;
            if (i == maxCol - 1)
            {
               color = bgColor;
            }
            frameBuf->drawPixel(x + i, y + j, color);
            pixelCol >>= 1;
         }
      }
   }
   else if (font->ByteOrder == FontByteOrder::Column)
   {
      fontData = &font->data[(c - ' ') * font->FontHeight];
      for (uint8_t i = 0; i < maxRow; i++)
      {
         auto pixelRow = fontData[i];
         auto mask = 0x8000;
         for (auto j = 0; j < maxCol; j++)
         {
            auto color = (pixelRow & mask) ? textColor : bgColor;
            frameBuf->drawPixel(x + j, y + i, color);
            mask >>= 1;
         }
      }
   }
}

uint32_t GUI::drawString(uint16_t xPos, uint16_t yPos, const char* pt, RGBColor textColor, RGBColor bgColor, bool lineWrap) const
{
   const char* orig = pt;

   auto MaxVisibleCharsPerRow = frameBuf->getWidth() / font->FontWidth;
   auto MaxVisibleRows = frameBuf->getHeight() / font->FontHeight;
   while (*pt)
   {
      if ((xPos > frameBuf->getWidth() && !lineWrap) || yPos > frameBuf->getHeight())
      {
         return pt - orig;
      }
      else if (*pt == '\n' || *pt == '\r')
      {
         xPos = 0;
         yPos += font->FontHeight;
      }
      else
      {
         if (xPos > frameBuf->getWidth() && lineWrap)
         {
            xPos = 0;
            yPos += font->FontHeight;
         }
         drawCharAtPosition(xPos, yPos, *pt, textColor, bgColor);
         xPos += font->FontWidth;
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
      drawString(rx, ry, gui_CurList->header);
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
            drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].text);
         }
         else
         {
            drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE);
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
               drawString(rx, ry + (i - gui_CurList->ItemsCount + maxC) * font->FontHeight, gui_CurList->items[i].text);
            }
            else
            {
               drawString(rx, ry + (i - gui_CurList->ItemsCount + maxC) * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE);
            }
         }
      }
      else if (gui_CurList->selectedItem < maxC / 2)
      {
         for (i = 0; i < maxC; i++)
         {
            if (i != gui_CurList->selectedItem)
            {
               drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].text);
            }
            else
            {
               drawString(rx, ry + i * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE);
            }
         }
      }
      else
      {
         for (i = gui_CurList->selectedItem - maxC / 2; i < gui_CurList->selectedItem - maxC / 2 + maxC; i++)
         {
            if (i != gui_CurList->selectedItem)
            {
               drawString(rx, ry + (i - gui_CurList->selectedItem + maxC / 2) * font->FontHeight, gui_CurList->items[i].text);
            }
            else
            {
               drawString(rx, ry + (i - gui_CurList->selectedItem + maxC / 2) * font->FontHeight, gui_CurList->items[i].getScrollOffset(), RGBColor::BLACK, RGBColor::WHITE);
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

