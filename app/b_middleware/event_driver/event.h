#ifndef __EVENT_H__
#define __EVENT_H__

/* ====================================== INCLDE HEADER ======================================= */
#include "stdint.h"
#include "stdbool.h"
#include "event_typedef.h"
/* ============================================================================================ */


/* =========================================== API ============================================ */
extern event_queue_t response_event_queue;
extern event_queue_t urc_event_queue;

bool push_event(event_queue_t* queue, event_t evt);
bool pop_event(event_queue_t* queue, event_t* evt);
bool event_queue_is_empty(event_queue_t* queue);
bool event_queue_is_full(event_queue_t* queue);
void event_queue_clear(event_queue_t* queue);
/* ============================================================================================ */

#endif /* __EVENT_H__ */