#include "exti_api.h"


/**
 * @brief   Initialize a button with EXTI using configuration struct
 * @param   cfg: Pointer to EXTI configuration
 */
void init_button_exti(exti_config_t* cfg){
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable SYSCFG clock for EXTI */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Map GPIO pin to EXTI line */
    SYSCFG_EXTILineConfig(cfg->port_source, cfg->pin_source);

    /* Configure EXTI */
    EXTI_InitStruct.EXTI_Line = (1 << cfg->exti_line);
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = cfg->trigger;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* Configure NVIC for EXTI */
    NVIC_InitStruct.NVIC_IRQChannel = cfg->irq_channel; 
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = cfg->preemption_priority;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = cfg->sub_priority;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}