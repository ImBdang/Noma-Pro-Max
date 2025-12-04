#ifndef __HARDWARE_H__
#define __HARDWARE_H__
/* A7600C1 */

/* =================================== INCLUDE HEADER ========================================== */
#include <string.h>
#include <stdbool.h>
#include <stm32f4xx_gpio.h>
/*<! HAL handmade */
#include "usart_api.h"
#include "systick_api.h"
#include "gpio_api.h"
#include "exti_api.h"
#include "clock_api.h"
/* ============================================================================================= */



/* =================================== PIN DEFINITION ========================================== */
/**
 * @brief   Define button on the modem
 */
#define BTN_1_PIN                       GPIO_Pin_0
#define BTN_1_PORT                      GPIOA


/**
 * @brief   Define PIN for STM32401 to communicate with A7600C1
 * 
 * @note    In this case my modem using USART1 of the STM MCU to communicate with the SIM module
 *          I will define also USART1 PIN and PWKEY PIN here, just everything that match to the SIM
 *          Also my module has only 1 led still active and its green, so i just keep the name
 */
#define LED_GREEN_NETWORK_PIN           GPIO_Pin_9
#define LED_GREEN_NETWORK_PORT          GPIOB
#define LED_4G_PIN                      LED_GREEN_NETWORK_PIN
#define LED_4G_PORT                     LED_GREEN_NETWORK_PORT

#define SIM_USART_Tx_Pin                GPIO_Pin_9
#define SIM_USART_Rx_Pin                GPIO_Pin_10
#define SIM_USART_GPIO                  GPIOA
#define SIM_USART_PORT                  USART1

#define SIM_STATUS_Pin                  GPIO_Pin_12
#define SIM_STATUS_GPIO                 GPIOA

#define SIM_PWKEY_Pin                   GPIO_Pin_8
#define SIM_PWKEY_GPIO                  GPIOA

/*<! PWKEY HIGH/LOW MACRO */
#define SIM_PWKEY_LOW()                 GPIO_ResetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin)
#define SIM_PWKEY_HIGH()                GPIO_SetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin)
#define SIM_STATUS_READ()               GPIO_ReadInputDataBit(SIM_STATUS_GPIO, SIM_STATUS_Pin)


 /**
  * @brief  Define PIN that for DEBUG, in my case is USART2 to communicate with laptop
  */
#define PC_STM_USART_TX_GPIO_PIN         GPIO_Pin_2
#define PC_STM_USART_RX_GPIO_PIN         GPIO_Pin_3
#define PC_STM_USART_GPIO                GPIOA
#define PC_STM_USART_PORT                USART2
/* ============================================================================================= */



/* ========================================= API =============================================== */
void hardware_init(void);
/* ============================================================================================= */


#endif /* __HARDWARE_H__ */

