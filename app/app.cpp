#include "app.h"

using namespace cmdc0de;

App::App() : CurrentState(0), LastRunTime(0), LastRunPerformance(0) {

}

ErrorType App::init() {
	return onInit();
}

ErrorType App::run() {
	LastRunTime = HAL_GetTick();
	ErrorType et = onRun();
	LastRunPerformance = HAL_GetTick()-LastRunTime;
	return et;
}
