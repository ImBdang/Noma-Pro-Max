#include "button_api.h"

/**
 * @brief   Initialize a button by configuring its GPIO
 * @param   GPIOx: GPIO port of the button (e.g., GPIOA, GPIOB)
 * @param   GPIO_InitStruct: Pointer to GPIO initialization structure containing
 *          pin, mode, pull-up/pull-down, speed, etc.
 * @note    This function wraps a lower-level GPIO initialization function (`init_gpio`)
 *          to simplify button initialization.
 */
void button_init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct){
    init_gpio(GPIOx, GPIO_InitStruct);
}


/**
 * @brief   Calculate how long a button has been pressed and update hold status flags
 * @param   btn: The button object containing press timestamps and thresholds
 * @note    - pressed_time is calculated as the difference between pressed_time_end and pressed_time_start
 *          - If the pressed time exceeds `so_long_time_hold` and it hasn't been detected yet, sets `so_long_detected`
 *          - Else if the pressed time exceeds `long_time_hold` and it hasn't been detected yet, sets `long_detected`
 * @note    This function does not handle button state reading; it only updates timing-based status flags.
 */
void button_check_time(button_t btn){
    btn.pressed_time = btn.pressed_time_end - btn.pressed_time_start;

    if (btn.pressed_time > btn.so_long_time_hold && !btn.so_long_detected) {
        btn.so_long_detected = 1;
    } 
    else if (btn.pressed_time > btn.long_time_hold && !btn.long_detected) {
        btn.long_detected = 1;
    }
    
}

/**
 * @brief   Based on the flag detected to return mode 
 * 
 * @retval  1 if so long detected true
 * @retval  2 if long detected true
 * @retval  0 if nothing detected
 */
uint8_t button_check_mode(button_t btn){
    if (btn.so_long_detected)
        return 1;
    else if (btn.long_detected)
        return 2;
    return 0;
}