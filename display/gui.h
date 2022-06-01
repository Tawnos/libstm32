#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include "display_device.h"
#include "fonts.h"

namespace cmdc0de {

   //running line
#define GUI_TickerSpeed 500
#define GUI_TickerEndDelay 3

   class GUITickerData
   {
   public:
      GUITickerData(const char* txt, uint8_t X, uint8_t Y, uint8_t W,
         uint8_t H);
      const char* text;
      uint8_t x, y, w, h, BorderSize, FontScalar;
      RGBColor bg, TextColor;
      uint32_t startTick;
   };

   class GUIListItemData
   {
   public:
      GUIListItemData(uint8_t id1, const char* msg, bool scroll,
         uint16_t timeBetwenScrolls) :
         id(id1),
         text(msg),
         Scrollable(scroll),
         TimeBetweenScroll(timeBetwenScrolls)
      {}
      GUIListItemData(uint8_t id, const char* msg) :
         id(id),
         text(msg)
      {
         setShouldScroll();
      }
      GUIListItemData() {}
      void set(uint8_t n, const char* msg)
      {
         id = n;
         text = msg;
      }
      uint16_t id{ 0 }; /*!< Item's id */
      const char* text{ 0 }; /*!< Item's text*/
      uint16_t Scrollable : 1{0};
      uint16_t TimeBetweenScroll : 12{1000};
      uint32_t LastScrollTime{ 0 };
      uint8_t LastScrollPosition{ 0 };
      const char* getScrollOffset();
      void setShouldScroll();
      bool shouldScroll();
      void resetScrollable()
      {
         Scrollable = 1;
         LastScrollTime = 0;
         LastScrollPosition = 0;
      }
   };

   class GUIListData
   {
   public:
      GUIListData(
         const char* h,
         GUIListItemData* is,
         uint8_t x,
         uint8_t y,
         uint8_t w,
         uint8_t height,
         uint8_t si,
         uint8_t ic
      ) : header(h),
         items(is),
         x(x),
         y(y),
         w(w),
         h(height),
         selectedItem(si),
         ItemsCount(ic)
      {}
      const char* header; /*!< Header*/
      GUIListItemData* items; /*!< Item's array*/
      uint16_t ItemsCount; /*!< Item's array*/
      uint8_t x, y, w, h;
      uint16_t selectedItem;
   };

   class GUI
   {
   public:
      GUI(FrameBuf* frameBuf, const FontDef_t* font = &Font_6x10) :
         frameBuf(frameBuf), font(font)
      {}
      bool init() { return true; }
      void drawTicker(GUITickerData* dt) const;
      uint8_t drawList(GUIListData* list) const;

      void fillScreen(const RGBColor& color) const
      {
         frameBuf->fillRec(0, 0, frameBuf->getWidth(), frameBuf->getHeight(), color);
      }
      void setTextColor(cmdc0de::RGBColor color)
      {
         CurrentTextColor = color;
      }

      const FontDef_t* getFont() const
      {
         return font;
      }
      void drawImage(uint16_t x, int16_t y, const DCImage& image) {
         frameBuf->drawImage(x, y, image);
      }
      void drawCharAtPosition(int16_t x, int16_t y, char c, const RGBColor& textColor, const RGBColor& bgColor, uint8_t size) const;
      uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor, const RGBColor& bgColor, uint8_t size, bool lineWrap) const;
      uint32_t drawString(uint16_t xPos, uint16_t yPos, const char* pt, const RGBColor& textColor, const RGBColor& bgColor, uint8_t size, bool lineWrap, uint8_t charsToRender) const;

      uint32_t drawStringOnLine(uint8_t line, const char* msg) const
      {
         return drawString(0, font->FontHeight * line, msg, CurrentTextColor, CurrentBGColor, 1, true);
      }

      uint32_t drawString(uint16_t x, uint16_t y, const char* pt) const
      {
         return drawString(x, y, pt, CurrentTextColor);
      }

      uint32_t drawString(uint16_t x, uint16_t y, const char* pt, const RGBColor& textColor) const
      {
         return drawString(x, y, pt, textColor, CurrentBGColor, 1, false);
      }
   private:
      FrameBuf* frameBuf;
      const FontDef_t* font;
      RGBColor CurrentTextColor{ cmdc0de::RGBColor::WHITE };
      RGBColor CurrentBGColor{ cmdc0de::RGBColor::BLACK };
   };

}
#endif
