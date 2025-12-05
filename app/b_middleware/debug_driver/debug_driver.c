#include "debug_driver.h"

/**
 * @brief   Send string
 */

// Trong debug_driver.c
void debug_printf(const char* fmt, ...){
    char buffer[256];  // Buffer để format string
    va_list args;
    
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    usart_sendstring(USART2, buffer);
}

/**
 * @brief   Function to create breakpoint for debug
 */
void breakp(void){
    DEBUG_PRINT("Breakpoint\r\n");
}