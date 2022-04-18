#include "app.h"

using namespace cmdc0de;

ErrorType App::init() {
	return onInit();
}

ErrorType App::run() {
	LastRunTime = HAL_GetTick();
	ErrorType et = onRun();
	LastRunPerformance = HAL_GetTick()-LastRunTime;
	return et;
}
