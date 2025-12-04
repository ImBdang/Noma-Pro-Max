#ifndef __GSM_H__
#define __GSM_H__

/* ====================================== INCLUDE HEADER ====================================== */
#include "stdint.h"
#include "stdbool.h"
#include "at_engine.h"
#include "gsm_callback.h"
#include "gsm_typedef.h"
#include "event_typedef.h"
#include "sim_typedef.h"
/* ============================================================================================ */

/* ====================================== DEFINTIONS ========================================== */
extern sim_state_t sim_state;
extern network_state_t network_state;
/* ============================================================================================ */


/* ====================================== TYPEDEF DEFINITION ================================== */
void gsm_process(void);
bool gsm_power_on(void);
bool gsm_power_off(void);
bool gsm_power_reset(void);
bool gsm_is_ready(void);
/* ============================================================================================ */

#endif /* __GSM_H__ */