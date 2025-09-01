#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "buzzer_beep_type.h"

#ifndef BUZZER_EVENT_QUEUE_HANDLER_H
#define BUZZER_EVENT_QUEUE_HANDLER_H

class buzzer_event_queue_handler_t
{
private:
    const char *TAG = "buzzer_event_queue_handler_t";
    QueueHandle_t queue;

public:
    void init();
    void add_to_queue(BUZZER_BEEP_TYPE beep_type);
    void task(void *pvParameter);
};

extern buzzer_event_queue_handler_t buzzer_event_queue_handler;

#endif // BUZZER_EVENT_QUEUE_HANDLER_H