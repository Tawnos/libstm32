#ifndef CMDC0DE_DISPLAY_MESSAGE_STATE_H
#define CMDC0DE_DISPLAY_MESSAGE_STATE_H

#include "state_base.h"

namespace cmdc0de {

class DisplayDevice;

class DisplayMessageState: public StateBase {
public:
	static const uint16_t DEFAULT_TIME_IN_STATE = 3000;
public:
	DisplayMessageState() = default;
	virtual ~DisplayMessageState() = default;
	void setMessage(const char *msg);
	void setTimeInState(uint16_t t) {
		TimeInState = t;
	}
	void setNextState(StateBase *b) {
		NextState = b;
	}
	StateBase *getNextState() {
		return NextState;
	}
	void setDisplay(DisplayDevice *dd) {Display = dd;}
	//const DisplayDevice *getDisplay() const {return Display;}
	DisplayDevice &getDisplay() {return *Display;}
	const StateBase *getNextState() const {return NextState;}
protected:
	virtual ErrorType onInit();
	virtual ReturnStateContext onRun();
	virtual ErrorType onShutdown();
private:
	char Message[64];
	uint16_t TimeInState{ DEFAULT_TIME_IN_STATE };
	StateBase* NextState{ 0 };
  DisplayDevice* Display{ 0 };
};


}

#endif
