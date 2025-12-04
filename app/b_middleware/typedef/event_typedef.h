#ifndef __EVENT_TYPEDEF_H__
#define __EVENT_TYPEDEF_H__

/* ====================================== INCLDE HEADER ======================================= */
#include "stdint.h"
/* ============================================================================================ */

/* ====================================== DEFINTIONS ========================================== */
#define EVENT_RESPONSE_QUEUE_SIZE           512
#define EVENT_URC_QUEUE_SIZE                512
/* ============================================================================================ */


/* ====================================== TYPEDEF DEFINITION ================================== */
typedef enum {
    RESPONSE = 0,
    URC
} event_queue_type_t;

typedef enum {
    EVT_OK = 0,
    EVT_ERR,
    EVT_TIMEOUT,

    EVT_SIM_RDY,
    EVT_SIM_PIN,
    EVT_SIM_PUK,

    EVT_CREG_NOT_REGISTERED,        // 0,0
    EVT_CREG_REGISTERED_HOME,       // 0,1
    EVT_CREG_SEARCHING,             // 0,2
    EVT_CREG_REG_DENIED,            // 0,3
    EVT_CREG_UNKNOWN,               // 0,4
    EVT_CREG_REGISTERED_ROAMING,    // 0,5
    EVT_CREG_REGISTERED_SMS,        // 0,6
    EVT_CREG_REGISTERED_SMS_ROAM    // 0,7

} response_event_t;

typedef enum {
    URC_SMS_NEW = 0,
    URC_SMS_RECEIVED,
    
    URC_HTTPACTION,
    URC_HTTPREAD,

    URC_RING
} urc_event_t;

typedef union  {
    response_event_t response;
    urc_event_t urc;
} event_t;


typedef struct {
    union {
        struct {
            response_event_t data[EVENT_RESPONSE_QUEUE_SIZE];
        } response;

        struct {
            urc_event_t data[EVENT_URC_QUEUE_SIZE];
        } urc;
    } queue;
    event_queue_type_t type;
    uint16_t queue_size;
    uint8_t head;
    uint8_t tail;
} event_queue_t;
/* ============================================================================================ */

#endif /* __EVENT_TYPEDEF_H__ */