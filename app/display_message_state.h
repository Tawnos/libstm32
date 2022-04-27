#ifndef CMDC0DE_DISPLAY_MESSAGE_STATE_H
#define CMDC0DE_DISPLAY_MESSAGE_STATE_H

#include "state_base.h"
#include <string.h>
#include <rgbcolor.h>
#include <display_device.h>

namespace cmdc0de {

  class DisplayDevice;

  class DisplayMessageState : public StateBase {
  public:
    static const uint16_t DEFAULT_TIME_IN_STATE = 3000;
  public:
    virtual ~DisplayMessageState() = default;

    void setMessage(const char* msg) { strncpy(&this->Message[0], msg, sizeof(this->Message)); }
    void setTimeInState(uint16_t t) { TimeInState = t; }

    const StateBase* getNextState() const { return NextState; }
    StateBase* getNextState() { return NextState; }
    void setNextState(StateBase* b) { NextState = b; }

    const DisplayDevice* getDisplay() const { return Display; }
    DisplayDevice* getDisplay() { return Display; }
    void setDisplay(DisplayDevice* dd) { Display = dd; }

  protected:
    virtual ErrorType onInit() {
      Display->fillScreen(RGBColor::BLACK);
      return ErrorType();
    }

    virtual ReturnStateContext onRun() {
      Display->drawString(0, 10, &this->Message[0], RGBColor::WHITE, RGBColor::BLACK, 1, true);
      if (timeInState() > TimeInState) { //|| kb.getLastKeyReleased() != QKeyboard::NO_PIN_SELECTED) {
        return ReturnStateContext(getNextState());
      }
      return ReturnStateContext(this);
    }

    virtual ErrorType onShutdown() { return ErrorType(); }
  private:
    char Message[64]{ 0 };
    uint16_t TimeInState{ DEFAULT_TIME_IN_STATE };
    StateBase* NextState{ 0 };
    DisplayDevice* Display{ 0 };
  };


}

#endif
