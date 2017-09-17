#ifndef _DCDARKNET_APP_H
#define _DCDARKNET_APP_H

#include "../config.h"
#include "../error_type.h"

namespace cmdc0de {

class StateBase;

class App {
public:
	ErrorType init();
	ErrorType run();
protected:
	ErrorType onInit();
	ErrorType onRun();
	StateBase *getCurrentState() {return CurrentState;}
	App();
private:
	StateBase *CurrentState;
	uint32_t LastRunTime;
	uint32_t LastRunPerformance;
};

}
#endif
