#ifndef __GSM_CALLBACK_H__
#define __GSM_CALLBACK_H__


/* ====================================== INCLUDE HEADER ======================================= */
#include "stdint.h"
#include "stdbool.h"
#include "callback_typedef.h"
#include "gsm_typedef.h"
#include "sim_typedef.h"
#include "string_handler.h"
#include "event.h"
#include "event_typedef.h"
#include "debug_driver.h"
/* ============================================================================================= */



/* ====================================== DEFINITIONS ========================================== */
/* ============================================================================================= */


/* ====================================== API ================================================== */
void gsm_basic_callback(response_status_t status, const char* line, uint32_t len);
void gsm_check_sim_callback(response_status_t status, const char* line, uint32_t len);
void gsm_reg_network_callback(response_status_t status, const char* line, uint32_t len);
/* ============================================================================================= */

#endif /* __GSM_CALLBACK_H__ */