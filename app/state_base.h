#ifndef STATE_BASE_H
#define STATE_BASE_H

#include "../error_type.h"

namespace cmdc0de {

class StateBase;

struct ReturnStateContext {
	ReturnStateContext(StateBase *next, const ErrorType &er) :
			NextMenuToRun(next), Err(er) {
	}
	ReturnStateContext(StateBase *n) :
			NextMenuToRun(n), Err() {
	}
	StateBase *NextMenuToRun;
	ErrorType Err;
};

class StateBase {
public:
	StateBase();
	template<typename RunContext>
	ReturnStateContext run(RunContext &rc);
	uint32_t timeInState();
	ErrorType shutdown();
	virtual ~StateBase();
protected:
	static const uint32_t INIT_BIT = 0x01;
	static const uint32_t DONT_RESET = 0x02;
	static const uint32_t SHIFT_FROM_BASE = 8;
	template<typename RunContext> virtual ErrorType onInit(RunContext &rc)=0;
	template<typename RunContext> virtual ReturnStateContext onRun(RunContext &rc)=0;
	virtual ErrorType onShutdown()=0;
	void setState(uint32_t n) {
		StateData |= n;
	}
	void clearState(uint32_t n) {
		StateData = (StateData & ~n);
	}
	bool checkState(uint32_t n) {
		return (StateData & n) != 0;
	}
	bool hasBeenInitialized() {
		return (StateData & INIT_BIT) != 0;
	}
	bool shouldReset() {
		return (StateData & DONT_RESET) == 0;
	}
	uint32_t getTimesRunCalledAllTime() {
		return TimesRunCalledAllTime;
	}
	uint32_t getTimesRunCalledSinceLastReset() {
		return TimesRunCalledSinceLastReset;
	}
private:
	template<typename RunContext> ErrorType init(RunContext &rc);
private:
	uint32_t StateData :8;
	uint32_t TimesRunCalledAllTime :24;
	uint32_t TimesRunCalledSinceLastReset;
	uint32_t StateStartTime;
};

//template<typename RunContext, int MAX_MESSAGE_SIZE = 64>
class DisplayMessageState: public StateBase {
public:
	static const uint16_t MAX_MESSAGE_SIZE = 64;
public:
	DisplayMessageState(uint16_t timeInState, StateBase *nextState);
	virtual ~DisplayMessageState();
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
protected:
	template<typename RunContext> virtual ErrorType onInit(RunContext &rc);
	template<typename RunContext> virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
private:
	char Message[MAX_MESSAGE_SIZE];
	uint16_t TimeInState;
	StateBase *NextState;
};

}
#endif
