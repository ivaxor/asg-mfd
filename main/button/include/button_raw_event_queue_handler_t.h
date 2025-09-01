#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "button_event_t.h"
#include "button_raw_event_t.h"

#ifndef BUTTON_RAW_EVENT_QUEUE_HANDLER_H
#define BUTTON_RAW_EVENT_QUEUE_HANDLER_H

class button_raw_event_queue_handler_t
{
private:
    const char *TAG = "button_raw_event_queue_handler_t";
    int64_t press_timestamps[GPIO_NUM_MAX] = {0};
    QueueHandle_t queue;

public:
    void init();
    void add_to_queue_isr(button_raw_event_t *button_raw_event);
    void task(void *pvParameter);
};

extern button_raw_event_queue_handler_t button_raw_event_queue_handler;

#endif // BUTTON_RAW_EVENT_QUEUE_HANDLER_H