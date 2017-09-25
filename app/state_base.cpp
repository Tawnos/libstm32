
#include <string.h>
#include "../config.h"
#include "state_base.h"

using namespace cmdc0de;

StateBase::StateBase() :
		StateData(0), TimesRunCalledAllTime(0), TimesRunCalledSinceLastReset(0), StateStartTime(0) {
}

StateBase::ReturnStateContext StateBase::run() {
	++TimesRunCalledAllTime;
	ReturnStateContext sr(this);
	if (!hasBeenInitialized()) {
		TimesRunCalledSinceLastReset = 0;
		ErrorType et = init();
		if (!et.ok()) {
			sr.NextMenuToRun = 0;
			sr.Err = et;
		}
	} else {
		++TimesRunCalledSinceLastReset;
		sr = onRun();
		if (sr.NextMenuToRun != this) {
			shutdown();
		}
	}
	return sr;
}

StateBase::~StateBase() {
}

ErrorType StateBase::init() {
	ErrorType et = onInit();
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

