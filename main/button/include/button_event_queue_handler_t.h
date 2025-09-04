#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "button_event_t.h"

#ifndef BUTTON_EVENT_QUEUE_HANDLER_H
#define BUTTON_EVENT_QUEUE_HANDLER_H

class button_event_queue_handler_t
{
private:
    static const char *TAG;
    static QueueHandle_t queue;

public:
    void init();
    void add_to_queue(button_event_t event);
    void task(void *pvParameter);
};

extern button_event_queue_handler_t button_event_queue_handler;

#endif // BUTTON_EVENT_QUEUE_HANDLER_H