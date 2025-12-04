#include "event.h"

/*<! FIXED INIT EVENT QUEUE */
event_queue_t response_event_queue = {
    .queue.response.data = {0},
    .queue_size = EVENT_RESPONSE_QUEUE_SIZE,
    .type = RESPONSE,
    .head = 0,
    .tail = 0
};
event_queue_t urc_event_queue = {
    .queue.urc.data = {0},
    .queue_size = EVENT_URC_QUEUE_SIZE,
    .type = URC,
    .head = 0,
    .tail = 0
};

/**
 * @brief   Set head and tail to 0
 */
void event_queue_clear(event_queue_t* queue)
{
    queue->head = 0;
    queue->tail = 0;
}

/**
 * @brief   Check queue is empty ?
 * 
 * @retval  True if is empty
 * @retval  False if is not empty
 */
bool event_queue_is_empty(event_queue_t* queue)
{
    return (queue->head == queue->tail);
}

/**
 * @brief   Check queue is full ?
 * 
 * @retval  True if is full
 * @retval  False if is not full
 */
bool event_queue_is_full(event_queue_t* queue)
{
    return ((queue->tail + 1) % queue->queue_size) == queue->head;
}

/**
 * @brief   Push new event to queue
 * 
 * @note    Switch case to check type of queue 
 *          to push event to right queue
 * 
 * @retval  True if success
 * @retval  False if not success
 */
bool push_event(event_queue_t* queue, event_t evt)
{
    if (event_queue_is_full(queue)) {
        return false;
    }

    switch (queue->type)
    {
        case RESPONSE:
            queue->queue.response.data[queue->tail] = evt.response;
            break;

        case URC:
            queue->queue.urc.data[queue->tail] = evt.urc;
            break;

        default:
            return false; 
    }

    queue->tail = (queue->tail + 1) % queue->queue_size;

    return true;
}

/**
 * @brief   Pop last event out of queue
 * 
 * @note    Switch case to check type of queue
 *          to pop
 * 
 * @retval  True if success
 * @retval  False if not success
 */
bool pop_event(event_queue_t* queue, event_t* evt)
{
    if (event_queue_is_empty(queue)) {
        return false;
    }

    switch (queue->type)
    {
        case RESPONSE:
            evt->response = queue->queue.response.data[queue->head];
            break;

        case URC:
            evt->urc = queue->queue.urc.data[queue->head];
            break;

        default:
            return false;   
    }

    queue->head = (queue->head + 1) % queue->queue_size;

    return true;
}
