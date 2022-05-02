#ifndef _DCDARKNET_APP_H
#define _DCDARKNET_APP_H

#include "../config.h"
#include "../error_type.h"
extern uint32_t HAL_GetTick();
extern void HAL_Delay(uint32_t);

namespace cmdc0de {

  class StateBase;

  class App {
  public:
    ErrorType init();
    ErrorType run();
  protected:
    virtual ~App() = default;

    virtual ErrorType onInit() = 0;
    virtual ErrorType onRun() = 0;
    StateBase* getCurrentState() { return CurrentState; }
    void setCurrentState(StateBase* cs) { CurrentState = cs; }
  private:
    StateBase* CurrentState{ nullptr };
    uint32_t LastRunTime{ 0 };
    uint32_t LastRunPerformance{ 0 };
  };

}
#endif
