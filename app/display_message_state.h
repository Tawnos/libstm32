#ifndef CMDC0DE_DISPLAY_MESSAGE_STATE_H
#define CMDC0DE_DISPLAY_MESSAGE_STATE_H

#include "../../menus/darknet7_base_state.h"
#include <string.h>
#include <rgbcolor.h>
#include <error_type.h>

namespace cmdc0de {

   class DisplayMessageState : public Darknet7BaseState
   {
   public:
      static const uint16_t DEFAULT_TIME_IN_STATE = 3000;
   public:
      using Darknet7BaseState::Darknet7BaseState;
      virtual ~DisplayMessageState() = default;

      void setMessage(const char* msg) { strncpy(&this->Message[0], msg, sizeof(this->Message)); }
      void setTimeInState(uint16_t t) { TimeInState = t; }

      const Darknet7BaseState* getNextState() const { return NextState; }
      Darknet7BaseState* getNextState() { return NextState; }
      void setNextState(Darknet7BaseState* b) { NextState = b; }

   protected:
      virtual ErrorType onInit() override;

      virtual Darknet7BaseState* onRun() override;

      virtual ErrorType onShutdown() override { return ErrorType(); }
   private:
      char Message[64]{ 0 };
      uint16_t TimeInState{ DEFAULT_TIME_IN_STATE };
      Darknet7BaseState* NextState{ 0 };
   };


}

#endif
