#ifndef __CALLBACK_TYPEDEF_H__
#define __CALLBACK_TYPEDEF_H__

/* ====================================== INCLDE HEADER ======================================= */
#include "stdint.h"
/* ============================================================================================ */

/* ====================================== DEFINTIONS ========================================== */
/* ============================================================================================ */


/* ====================================== TYPEDEF DEFINITION ================================== */
typedef enum {
    OK_RESP,
    ERROR_RESP,
    TIMEOUT_RESP,
    PRIMARY_RESP,
    INTERMEDIATE
} response_status_t;

typedef void (*gsm_callback)(response_status_t status, const char* line, uint32_t len);
/* ============================================================================================ */

#endif /* __CALLBACK_TYPEDEF_H__ */