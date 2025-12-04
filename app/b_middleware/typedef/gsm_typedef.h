#ifndef __GSM_TYPEDEF_H__
#define __GSM_TYPEDEF_H__

/* ====================================== INCLUDE HEADER ======================================= */
#include "stdint.h"
/* ============================================================================================ */

/* ====================================== DEFINITIONS ========================================== */
/* ============================================================================================ */

/* ====================================== TYPEDEF DEFINITION ================================== */
/* ============================================================================================ */

typedef enum {
    GSM_DECISION_STATE,
    GSM_POWER_OFF_STATE,
    GSM_POWER_ON_STATE,
    GSM_SYNC_AT_STATE,
    GSM_CONFIG_STATE,
    GSM_CHECK_SIM_STATE,
    GSM_REG_NETWORK_STATE,
    GSM_ATTACH_PSD,
    GSM_ACTIVATE_PDP,
    GSM_ERROR,
    GSM_READY
} gsm_state_t;

typedef struct {
    bool power_off;
    bool power_on;
    bool sync_at;
    bool config;
    bool check_sim;
    bool reg_network;
    bool attach_psd;
    bool activate_pdp;
    bool error;
    bool ready;
} gsm_decision_flag_t;

typedef enum {
    NET_NOT_REGISTERED,       // stat = 0
    NET_REGISTERED_HOME,      // stat = 1
    NET_SEARCHING,            // stat = 2
    NET_REG_DENIED,           // stat = 3
    NET_UNKNOWN,              // stat = 4
    NET_REGISTERED_ROAMING,   // stat = 5
    NET_REGISTERED_SMS,       // stat = 6
    NET_REGISTERED_SMS_ROAM   // stat = 7
} network_state_t;

/* ============================================================================================ */

#endif /* __GSM_TYPEDEF_H__ */