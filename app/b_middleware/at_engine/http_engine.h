#ifndef __HTTP_ENGINE_H__
#define __HTTP_ENGINE_H__

/* ====================================== INCLUDE HEADER ======================================= */
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "debug_driver.h"
#include "at_engine.h"
#include "http_typedef.h"
#include "gsm_callback.h"
/* ============================================================================================= */



/* ====================================== VARIABLES ============================================ */
extern uint8_t http_read_buff[HTTP_READ_BUFFER];
extern uint32_t http_data_len;
extern uint32_t reading_chunk;
/* ============================================================================================= */


/* ====================================== API ================================================== */
bool http_init(void);
bool http_term(void);
bool http_seturl(const char* url);
bool http_action(uint8_t action);
bool http_read(uint32_t offset, uint32_t chunk);
bool http_is_ready(void);
/* ============================================================================================= */

#endif /* __HTTP_ENGINE_H__ */