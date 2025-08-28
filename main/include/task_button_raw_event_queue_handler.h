#include "freertos/FreeRTOS.h"

#ifndef TASK_BUTTON_RAW_EVENT_QUEUE_HANDLER_H
#define TASK_BUTTON_RAW_EVENT_QUEUE_HANDLER_H

typedef struct {
    uint32_t gpio_num;
    bool pressed;
    int64_t timestamp;
} button_raw_event_t;

extern QueueHandle_t button_raw_event_queue;

void task_button_raw_event_queue_handler(void *pvParameter);

#endif // TASK_BUTTON_RAW_QUEUE_HANDLER_H