#ifndef __USART_M__
#define __USART_M__

/* Include header ---------------------------------------------------------- */
#include "stdint.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/* Struct ------------------------------------------------------------------ */
/**
 * @brief   Struct to init usart
 * 
 * @note    USARTx following by USART 1, 2, 3 ...
 *          TX_port following by TX gpoi port that you choose
 *          TX_PinSource following by Pin Source for TX gpio (not mask)
 *          TX_AF alertnate mode for usart as like USART1_AF
 *          
 *          The same for RX, following by note above
 * 
 *          baudrate you choose
 */
typedef struct {
    USART_TypeDef* USARTx;
    GPIO_TypeDef* TX_Port;
    uint16_t TX_Pin;
    uint8_t TX_PinSource;  
    uint8_t TX_AF;
    
    GPIO_TypeDef* RX_Port;
    uint16_t RX_Pin;
    uint8_t RX_PinSource;  
    uint8_t RX_AF;
    
    uint32_t baudrate;
} usart_c;


/* API declrations --------------------------------------------------------- */
void usart_init(usart_c* cfg); 
void usart_sendstring(USART_TypeDef *USARTx, const char *str);
uint8_t usart_receive(USART_TypeDef* USARTx);

#endif /* __USART__ */