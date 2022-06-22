#include "display_message_state.h"
#include <error_type.h>
#include <darknet7.h>

cmdc0de::ErrorType cmdc0de::DisplayMessageState::onInit()
{
    darknet->getGUI()->fillScreen(RGBColor::BLACK);
    return ErrorType();
}

Darknet7BaseState*  cmdc0de::DisplayMessageState::onRun()
{
    darknet->getGUI()->drawString(0, 10, &this->Message[0]);
    if (timeInState() > TimeInState)
    { //|| kb.getLastKeyReleased() != QKeyboard::NO_PIN_SELECTED) {
        return NextState;
    }
    return this;
}
