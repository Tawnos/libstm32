
#include <string.h>
#include "state_base.h"

using namespace cmdc0de;

StateBase::StateBase() :
		StateData(0), TimesRunCalledAllTime(0), TimesRunCalledSinceLastReset(0), StateStartTime(0) {
}

StateBase::ReturnStateContext StateBase::run(RunContext &rc) {
	++TimesRunCalledAllTime;
	ReturnStateContext sr(this);
	if (!hasBeenInitialized()) {
		TimesRunCalledSinceLastReset = 0;
		ErrorType et = init(rc);
		if (!et.ok()) {
			sr.NextMenuToRun = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), et.getMessage(),
					10000);
		}
	} else {
		++TimesRunCalledSinceLastReset;
		sr = onRun(rc);
		if (sr.NextMenuToRun != this) {
			shutdown();
		}
	}
	return sr;
}

StateBase::~StateBase() {
}

ErrorType StateBase::init(RunContext &rc) {
	ErrorType et = onInit(rc);
	if (et.ok()) {
		setState(INIT_BIT);
		StateStartTime = HAL_GetTick();
	}
	return et;
}
ErrorType StateBase::shutdown() {
	ErrorType et = onShutdown();
	clearState(INIT_BIT);
	StateStartTime = 0;
	return et;
}

uint32_t StateBase::timeInState() {
	return HAL_GetTick() - StateStartTime;
}

//=======================================================================
DisplayMessageState::DisplayMessageState(uint16_t timeInState, StateBase *nextState) :
		TimeInState(timeInState), NextState(nextState) {
}

DisplayMessageState::~DisplayMessageState() {
}

ErrorType DisplayMessageState::onInit(RunContext &rc) {
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	return ErrorType();
}

void DisplayMessageState::setMessage(const char *msg) {
	strncpy(&this->Message[0], msg, sizeof(this->Message));
}

ReturnStateContext DisplayMessageState::onRun(RunContext &rc) {
	rc.getDisplay().drawString(0, 10, &this->Message[0], RGBColor::WHITE, RGBColor::BLACK, 1, true);
	if (timeInState() > TimeInState) { //|| kb.getLastKeyReleased() != QKeyboard::NO_PIN_SELECTED) {
		return ReturnStateContext(StateFactory::getMenuState());
	}
	return ReturnStateContext(this);
}

ErrorType DisplayMessageState::onShutdown() {
	return ErrorType();
}
