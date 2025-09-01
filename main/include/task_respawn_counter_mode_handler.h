#include "freertos/FreeRTOS.h"

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

#endif // TASK_RESPAWN_COUNTER_MODE_H