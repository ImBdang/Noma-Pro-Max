#ifndef __DEBUG_H__
#define __DEBUG_H__

/* ====================================== INCLUDE HEADER ======================================= */
#include "stdint.h"
#include "usart_api.h"
#include <stdarg.h>
#include <stdio.h>
/* ============================================================================================= */



/* ====================================== DEFINITIONS ========================================== */
#define DEBUG_PRINT(fmt, ...) debug_printf(fmt, ##__VA_ARGS__)

// Nếu muốn có thể tắt debug, dùng:
// #ifdef DEBUG_ENABLE
//     #define DEBUG_PRINT(fmt, ...) debug_printf(fmt, ##__VA_ARGS__)
// #else
//     #define DEBUG_PRINT(fmt, ...)
// #endif
/* ============================================================================================= */



/* ========================================= API =============================================== */
void debug_printf(const char* fmt, ...);
void breakp(void);
/* ============================================================================================= */

#endif /* __DEBUG_H__ */