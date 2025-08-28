#include "freertos/FreeRTOS.h"

#ifndef TASK_BUZZER_EVENT_QUEUE_HANDLER_H
#define TASK_BUZZER_EVENT_QUEUE_HANDLER_H

typedef enum
{
    RESPAWN_NO_TOKENS,
    RESPAWN_TOKEN_DECREASE,
    RESPAWN_BATCH,
    RESPAWN_SETUP_MODE_ENABLED,
} BEEP_TYPE;

extern QueueHandle_t buzzer_event_queue;

void task_buzzer_event_queue_handler(void *pvParameter);

#endif // TASK_BUZZER_EVENT_QUEUE_HANDLER_H