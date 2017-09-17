#ifndef CMDC0DE_UTILITY_PIN
#define CMDC0DE_UTILITY_PIN

#include "../config.h"

namespace cmdc0de {

struct PinConfig {
	GPIO_TypeDef *Port;
	uint16_t Pin;
};

}

#endif
