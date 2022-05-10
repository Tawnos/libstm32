#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include "display_device.h"
#include "fonts.h"

namespace cmdc0de {

  //running line
#define GUI_TickerSpeed 500
#define GUI_TickerEndDelay 3

  class GUITickerData {
  public:
    GUITickerData(const char* txt, uint8_t X, uint8_t Y, uint8_t W,
      uint8_t H);
    const char* text;
    uint8_t x, y, w, h, BorderSize, FontScalar;
    RGBColor bg, TextColor;
    uint32_t startTick;
  };

  class GUIListItemData {
  public:
    GUIListItemData(uint8_t id1, const char* msg, bool scroll,
      uint16_t timeBetwenScrolls) :
      id(id1), 
      text(msg), 
      Scrollable(scroll), 
      TimeBetweenScroll(timeBetwenScrolls) { }
    GUIListItemData(uint8_t id, const char* msg) :
      id(id),
      text(msg) {
       setShouldScroll();
    }
    GUIListItemData() {}
    void set(uint8_t n, const char* msg) {
      id = n;
      text = msg;
    }
    uint16_t id{0}; /*!< Item's id */
    const char* text{0}; /*!< Item's text*/
    uint16_t Scrollable : 1{0};
    uint16_t TimeBetweenScroll : 12{1000};
    uint32_t LastScrollTime{0};
    uint8_t LastScrollPosition{0};
    const char* getScrollOffset();
    void setShouldScroll();
    bool shouldScroll();
    void resetScrollable() {
      Scrollable = 1;
      LastScrollTime = 0;
      LastScrollPosition = 0;
    }
  };

  class GUIListData {
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
        ItemsCount(ic) { }
    const char* header; /*!< Header*/
    GUIListItemData* items; /*!< Item's array*/
    uint16_t ItemsCount; /*!< Item's array*/
    uint8_t x, y, w, h;
    uint16_t selectedItem;
  };

  class GUI {
  public:
    GUI(DisplayDevice* display) :
      Display(display) {}
    bool init();
    void drawTicker(GUITickerData* dt);
    uint8_t drawList(GUIListData* list) const;

  private:
    DisplayDevice* Display;
  };

}
#endif
