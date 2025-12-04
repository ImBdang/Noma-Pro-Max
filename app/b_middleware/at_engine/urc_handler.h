#ifndef __URC_HANDLER_H__
#define __URC_HANDLER_H__

/* ====================================== INCLUDE HEADER ======================================= */
#include "at_typedef.h"
#include "at_engine.h"
#include "callback_typedef.h"
#include "stdbool.h"
#include "stdint.h"
#include "event.h"
#include "string_handler.h"
#include "debug_driver.h"
#include "event_typedef.h"
/* ============================================================================================= */



/* ====================================== DEFINITIONS ========================================== */
extern event_queue_t urc_event_queue;
extern uint32_t http_data_len;
/* ============================================================================================= */


/* ====================================== API ================================================== */
void urc_process(const char* urc);
/* ============================================================================================= */

#endif /* __URC_HANDLER_H__ */