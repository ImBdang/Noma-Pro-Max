#ifndef __EXTI_M__
#define __EXTI_M__

/* Include header ---------------------------------------------------------- */
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"

/* STRUCT CONFIG ----------------------------------------------------------- */
typedef struct {
    GPIO_TypeDef* GPIOx;          /*!< GPIO port of the button */
    uint8_t port_source;          /*!< Port source*/
    uint8_t pin_source;           /*!< Pin source on port (0..15) */
    uint8_t exti_line;            /*!< EXTI line (0..15) */
    IRQn_Type irq_channel;        /*!< Interrupt request number for the EXTI line */
    uint8_t trigger;              /*!< EXTI trigger (Rising, Falling, Rising_Falling) */
    uint8_t preemption_priority;  /*!< NVIC preemption priority */
    uint8_t sub_priority;         /*!< NVIC subpriority */
} exti_config_t;

/* API declarations -------------------------------------------------------- */
void init_button_exti(exti_config_t* cfg);

#endif /* __EXTI__ */