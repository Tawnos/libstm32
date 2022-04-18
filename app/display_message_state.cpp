#include "display_message_state.h"
#include "../display/display_device.h"

using namespace cmdc0de;

//=======================================================================

ErrorType DisplayMessageState::onInit() {
	getDisplay().fillScreen(RGBColor::BLACK);
	return ErrorType();
}

void DisplayMessageState::setMessage(const char *msg) {
	strncpy(&this->Message[0], msg, sizeof(this->Message));
}

StateBase::ReturnStateContext DisplayMessageState::onRun() {
	getDisplay().drawString(0, 10, &this->Message[0], RGBColor::WHITE, RGBColor::BLACK, 1, true);
	if (timeInState() > TimeInState) { //|| kb.getLastKeyReleased() != QKeyboard::NO_PIN_SELECTED) {
		return ReturnStateContext(getNextState());
	}
	return ReturnStateContext(this);
}

ErrorType DisplayMessageState::onShutdown() {
	return ErrorType();
}
