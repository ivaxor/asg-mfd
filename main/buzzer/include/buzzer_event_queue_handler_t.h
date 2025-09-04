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
    static const char *TAG;
    static QueueHandle_t queue;
    buzzer_event_queue_handler_t();

public:
    static void init();
    static void add_to_queue(BUZZER_BEEP_TYPE beep_type);
    static void task(void *pvParameter);
};

#endif // BUZZER_EVENT_QUEUE_HANDLER_H