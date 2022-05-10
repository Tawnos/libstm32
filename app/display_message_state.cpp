#include "display_message_state.h"
#include <error_type.h>
#include <darknet7.h>

cmdc0de::ErrorType cmdc0de::DisplayMessageState::onInit()
{
    darknet->getDisplay()->fillScreen(RGBColor::BLACK);
    return ErrorType();
}

Darknet7BaseState*  cmdc0de::DisplayMessageState::onRun()
{
    darknet->getDisplay()->drawString(0, 10, &this->Message[0], RGBColor::WHITE, RGBColor::BLACK, 1, true);
    if (timeInState() > TimeInState)
    { //|| kb.getLastKeyReleased() != QKeyboard::NO_PIN_SELECTED) {
        return NextState;
    }
    return this;
}
