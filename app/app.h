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
