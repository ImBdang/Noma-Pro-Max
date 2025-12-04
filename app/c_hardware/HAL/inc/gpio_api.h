#ifndef __GPIO_M__
#define __GPIO_M__

/* Include header ---------------------------------------------------------- */
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/* API declarations -------------------------------------------------------- */
void init_gpio(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

#endif /* __GPIO__ */