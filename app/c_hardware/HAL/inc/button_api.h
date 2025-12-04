#ifndef __BUTTON_M__
#define __BUTTON_M__

#include "stm32f4xx_gpio.h"
#include "gpio_api.h"

/* TYPDEF STRUCT ---------------------------------------------------------------------- */
typedef struct {
    uint32_t so_long_time_hold;
    uint32_t long_time_hold;
    uint8_t pressed;
    uint32_t pressed_time_start;
    uint32_t pressed_time_end;       
    uint32_t pressed_time;   
    uint8_t long_detected; 
    uint8_t so_long_detected;
} button_t;


void button_init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void button_check_time(button_t btn);
uint8_t button_check_mode(button_t btn);

#endif /* __BUTTON__ */