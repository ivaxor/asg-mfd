#include "task_button_event_queue_handler.h"

#ifndef TASK_RESPAWN_COUNTER_MODE_H
#define TASK_RESPAWN_COUNTER_MODE_H

typedef struct
{
    uint8_t priority;
    bool enabled;
    uint16_t min;
    uint16_t max;
    uint16_t batch_size;
} respawn_policy_t;

void task_respawn_counter_mode_handler(void *pvParameter);
void respawn_mode_button_event_handler(button_event_t button_event);

#endif // TASK_RESPAWN_COUNTER_MODE_H