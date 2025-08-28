#include "freertos/FreeRTOS.h"

#ifndef TASK_BUTTON_EVENT_QUEUE_HANDLER_H
#define TASK_BUTTON_EVENT_QUEUE_HANDLER_H

typedef enum
{
    PRESSED,
    DEPRESSED,
} BUTTON_EVENT_STATE;

typedef struct
{
    uint32_t gpio_num;
    BUTTON_EVENT_STATE state;
    int64_t durationMS;
} button_event_t;

extern QueueHandle_t button_event_queue;

void task_button_event_queue_handler(void *pvParameter);

#endif // TASK_BUTTON_QUEUE_HANDLER_H