#include "hardware.h"

/* ================================== STATIC DECLARATIONS =================================== */
static void modem_init_pc_stm_usart_hw(void);
static void modem_init_sim_usart_hw(void);
static void modem_init_led_hw(void);
static void modem_init_button_hw(void);
static void modem_init_pwkey_gpio_hw(void);
/* =========================================================================================== */

/**
 * @brief   Init usart that connect pc to mcu 
 * 
 * @note    USART2 stm32f401ccu6
 *          115200
 */
void modem_init_pc_stm_usart_hw(void){
    usart_c usart_config = {
        .USARTx = PC_STM_USART_PORT,
        .TX_Port = PC_STM_USART_GPIO,
        .TX_Pin = PC_STM_USART_TX_GPIO_PIN,
        .TX_PinSource = GPIO_PinSource2,
        .TX_AF = GPIO_AF_USART2,
        .RX_Port = PC_STM_USART_GPIO,
        .RX_Pin = PC_STM_USART_RX_GPIO_PIN,
        .RX_PinSource = GPIO_PinSource3,
        .RX_AF = GPIO_AF_USART2,
        .baudrate = 115200
    };
    usart_init(&usart_config);

    while (USART_GetFlagStatus(PC_STM_USART_PORT, USART_FLAG_RXNE)) {
        (void)USART_ReceiveData(PC_STM_USART_PORT);
    }
}

/**
 * @brief   Init usart that connect to the SIM module
 * 
 * @note    Stm32f401ccu6 to A7600C1
 *          115200
 *          This one used to receive data too so it enable INTERRUPT (RXNE)
 */
void modem_init_sim_usart_hw(void){
    usart_c usart_config = {
        .USARTx = SIM_USART_PORT,
        .TX_Port = SIM_USART_GPIO,
        .TX_Pin = SIM_USART_Tx_Pin,
        .TX_PinSource = GPIO_PinSource9,
        .TX_AF = GPIO_AF_USART1,
        .RX_Port = SIM_USART_GPIO,
        .RX_Pin = SIM_USART_Rx_Pin,
        .RX_PinSource = GPIO_PinSource10,
        .RX_AF = GPIO_AF_USART1,
        .baudrate = 115200
    };
    usart_init(&usart_config);

    while (USART_GetFlagStatus(SIM_USART_PORT, USART_FLAG_RXNE)) {
        (void)USART_ReceiveData(SIM_USART_PORT);
    }

    /*<! IRQ of the USART */
    NVIC_ClearPendingIRQ(USART1_IRQn);
    USART_ITConfig(SIM_USART_PORT, USART_IT_RXNE, ENABLE);
    NVIC_InitTypeDef nvic_config = {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0,
        .NVIC_IRQChannelSubPriority = 0,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_Init(&nvic_config);
}

/**
 * @brief   Init LED 
 * 
 * @note    Mode out, 25MHz, no PuPd
 */
void modem_init_led_hw(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = LED_4G_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LED_4G_PORT, &GPIO_InitStruct);
}


/**
 * @brief   Init button
 * 
 * @note    
 */
void modem_init_button_hw(void){

}


/**
 * @brief   Init PWKEY status gpipo
 * 
 * @note    This one init a gpio pin that connect to PWKEY of the SIM MODULE
 *          There are 2 GPIO Pin here, 1 is PWKEY used to push HIGH/LOW to turn SIM on/off
 *          Other one is used for read status of the PWKEY 
 * 
 * @note    PWKEY mode out, 50MHz, no PuPd
 *          STATUS_SIM mode in, no PuPd
 */
void modem_init_pwkey_gpio_hw(void){
    GPIO_InitTypeDef gpio_c = {
        .GPIO_Pin = SIM_PWKEY_Pin,
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_PuPd = GPIO_PuPd_NOPULL 
    };
    init_gpio(SIM_PWKEY_GPIO, &gpio_c);

    gpio_c.GPIO_Pin = SIM_STATUS_Pin;
    gpio_c.GPIO_Mode = GPIO_Mode_IN;
    gpio_c.GPIO_PuPd = GPIO_PuPd_DOWN;
    init_gpio(SIM_STATUS_GPIO, &gpio_c);
}

/**
 * @brief   Recall these init func above
 */
void hardware_init(void){
    clock_init_64MHz_HSI();
    systick_init(1000);
    
    modem_init_pc_stm_usart_hw();
    modem_init_sim_usart_hw();
    modem_init_led_hw();
    modem_init_button_hw();
    modem_init_pwkey_gpio_hw();
    //GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
}