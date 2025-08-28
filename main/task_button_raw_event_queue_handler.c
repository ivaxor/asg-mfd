#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "include/task_button_raw_event_queue_handler.h"
#include "include/task_button_event_queue_handler.h"

static const char *TAG = "task_button_raw_event_queue_handler";

QueueHandle_t button_raw_event_queue;

static int64_t button_raw_event_press_timestamps[27] = {0};
void button_raw_event_handler(button_raw_event_t button_raw_event)
{
    button_event_t button_event;
    int64_t lastPressTimestamp = button_raw_event_press_timestamps[button_raw_event.gpio_num];

    switch (button_raw_event.pressed)
    {
    case 0:
        if (lastPressTimestamp == 0)
            return;

        button_event.state = DEPRESSED;
        button_event.durationMS = (button_raw_event.timestamp - lastPressTimestamp) / 1000;

        button_raw_event_press_timestamps[button_raw_event.gpio_num] = 0;
        break;

    case 1:
        if (lastPressTimestamp != 0)
            return;

        button_event.state = PRESSED;
        button_event.durationMS = 0;

        button_raw_event_press_timestamps[button_raw_event.gpio_num] = button_raw_event.timestamp;
        break;
    }

    button_event.gpio_num = button_raw_event.gpio_num;
    if (xQueueSend(button_event_queue, &button_event, 0) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send button event to queue");
        return;
    }
}

void task_button_raw_event_queue_handler(void *pvParameter)
{
    button_raw_event_queue = xQueueCreate(10, sizeof(button_raw_event_t));
    if (button_raw_event_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create button raw event queue");
        return;
    }

    button_raw_event_t button_raw_event;
    while (1)
    {
        xQueueReceive(button_raw_event_queue, &button_raw_event, portMAX_DELAY);
        button_raw_event_handler(button_raw_event);
    }
}