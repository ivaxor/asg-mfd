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
    static const char *TAG;
    static int64_t press_timestamps[GPIO_NUM_MAX];
    static int64_t click_timestamps[GPIO_NUM_MAX];
    static QueueHandle_t queue;
    button_raw_event_queue_handler_t();

public:
    static void init();
    static void add_to_queue_isr(button_raw_event_t *button_raw_event);
    static void task(void *pvParameter);
};

#endif // BUTTON_RAW_EVENT_QUEUE_HANDLER_H