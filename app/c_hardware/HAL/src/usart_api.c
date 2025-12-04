#include "usart_api.h"

/**
 * @brief   Init usart following by usart_c struct
 * 
 * @param cfg   Pointer to usart_c variable 
 * 
 * @note    For more information about usart_c please read usart.h
 */
void usart_init(usart_c* cfg) {
    // Enable USART clock
    if (cfg->USARTx == USART1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    }
    else if (cfg->USARTx == USART2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    }
    else if (cfg->USARTx == USART3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    }
    else if (cfg->USARTx == UART4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    }
    else if (cfg->USARTx == UART5) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    }
    else if (cfg->USARTx == USART6) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
    }
    else if (cfg->USARTx == UART7) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
    }
    else if (cfg->USARTx == UART8) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);
    }

    // Enable GPIO clocks for TX port
    if (cfg->TX_Port == GPIOA) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    }
    else if (cfg->TX_Port == GPIOB) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    }
    else if (cfg->TX_Port == GPIOC) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    }
    else if (cfg->TX_Port == GPIOD) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    }
    else if (cfg->TX_Port == GPIOE) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    }
    else if (cfg->TX_Port == GPIOF) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    }
    else if (cfg->TX_Port == GPIOG) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    }

    // Enable GPIO clocks for RX port 
    if (cfg->RX_Port != cfg->TX_Port) {
        if (cfg->RX_Port == GPIOA) {
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        }
        else if (cfg->RX_Port == GPIOB) {
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        }
        else if (cfg->RX_Port == GPIOC) {
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        }
        else if (cfg->RX_Port == GPIOD) {
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        }
        else if (cfg->RX_Port == GPIOE) {
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        }
        else if (cfg->RX_Port == GPIOF) {
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
        }
        else if (cfg->RX_Port == GPIOG) {
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
        }
    }

    // Configure TX pin
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = cfg->TX_Pin;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(cfg->TX_Port, &gpio);
    GPIO_PinAFConfig(cfg->TX_Port, cfg->TX_PinSource, cfg->TX_AF);

    // Configure RX pin
    gpio.GPIO_Pin = cfg->RX_Pin;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(cfg->RX_Port, &gpio);
    GPIO_PinAFConfig(cfg->RX_Port, cfg->RX_PinSource, cfg->RX_AF);

    // Configure USART
    USART_InitTypeDef usart_init_struct;
    USART_StructInit(&usart_init_struct); 
    usart_init_struct.USART_BaudRate = cfg->baudrate;
    usart_init_struct.USART_WordLength = USART_WordLength_8b;
    usart_init_struct.USART_StopBits = USART_StopBits_1;
    usart_init_struct.USART_Parity = USART_Parity_No;
    usart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(cfg->USARTx, &usart_init_struct);

    // Enable USART
    USART_Cmd(cfg->USARTx, ENABLE);
}

/**
 * @brief   Send each char in a string
 * 
 * @param   USARTx: USART 1, 2, 3, ...
 * @param   str: String that you need to send
 */
void usart_sendstring(USART_TypeDef *USARTx, const char *str) {
    while (*str) {
        USART_SendData(USARTx, *str++);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}

/**
 * @brief   Receive each byte
 * 
 * @retval  Byte that you receive
 */
uint8_t usart_receive(USART_TypeDef* USARTx) {
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USARTx);
}