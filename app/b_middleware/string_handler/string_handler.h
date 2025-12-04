#ifndef __STRING_HANDLER_H__
#define __STRING_HANDLER_H__

/* ====================================== INCLUDE HEADER ======================================= */
#include "string.h"
#include "stdint.h"
/* ============================================================================================= */



/* ====================================== DEFINITIONS ========================================== */
/* ============================================================================================= */


/* ====================================== API ================================================== */
void primary_dispatch(const char* str, char* result);
void new_sms_dispatch(const char* str, char* id_out);
void http_action_dispatch(const char* str, uint8_t* method, char status[4], uint32_t* len);
uint32_t httpread_dispatch(const char* str);
uint32_t fast_atoi(const char *s);
void fast_itoa(uint32_t value, char *buf);
/* ============================================================================================= */

#endif /* __STRING_HANDLER_H__ */