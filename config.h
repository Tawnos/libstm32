#ifndef LIBSTM32_CONFIG_H
#define LIBSTM32_CONFIG_H

#ifdef STM32F10X_MD
	#include <stm32f1xx_hal.h>
	#include <stm32f1xx_hal_gpio.h>
	#include <stm32f1xx_hal_tim.h>
	#include <stm32f1xx_hal_tim_ex.h>
#define DMA_Channel_StreamTypeDef DMA_Channel_TypeDef
#elif defined (STM32L452xx)
	#include <stm32l4xx_hal.h>
	#include <stm32l4xx_hal_gpio.h>
	#include <stm32l4xx_hal_tim.h>
	#include <stm32l4xx_hal_tim_ex.h>
#define DMA_Channel_StreamTypeDef DMA_Channel_TypeDef
#elif defined (STM32F411xE)
	#include <stm32f4xx_hal.h>
	#include <stm32f4xx_hal_gpio.h>
	#include <stm32f4xx_hal_tim.h>
	#include <stm32f4xx_hal_tim_ex.h>
	#include <stm32f4xx_hal_dma.h>
	#include <stm32f4xx_hal_dma_ex.h>
#define DMA_Channel_StreamTypeDef DMA_Stream_TypeDef
#elif defined (STM32F302x8)
	#include <stm32f3xx_hal.h>
	#include <stm32f3xx_hal_gpio.h>
	#include <stm32f3xx_hal_tim.h>
	#include <stm32f3xx_hal_tim_ex.h>
	#include <stm32f3xx_hal_dma.h>
	#include <stm32f3xx_hal_dma_ex.h>
#define DMA_Channel_StreamTypeDef DMA_Channel_TypeDef
#elif defined (STM32G031xx)
	#include <stm32g0xx_hal.h>
	#include <stm32g0xx_hal_gpio.h>
	#include <stm32g0xx_hal_tim.h>
	#include <stm32g0xx_hal_tim_ex.h>
	#include <stm32g0xx_hal_dma.h>
	#include <stm32g0xx_hal_dma_ex.h>
#ifdef HAL_I2C_MODULE_ENABLED
	#include <stm32g0xx_hal_i2c.h>
#endif
#define DMA_Channel_StreamTypeDef DMA_Channel_TypeDef
#endif

#include "error_type.h"

#endif
