#include "debug_driver.h"

/**
 * @brief   Send string
 */
void usart_debug(const char* s){
    usart_sendstring(USART2, s);
}

/**
 * @brief   Function to create breakpoint for debug
 */
void breakp(void){
    DEBUG_PRINT("Breakpoint\r\n");
}