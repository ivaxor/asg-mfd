#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "include/task_button_event_queue_handler.h"

static const char *TAG = "task_button_event_queue_handler";

QueueHandle_t button_event_queue;

static int64_t button_event_press_timestamps[27] = {0};
void button_event_handler(button_event_t button_event)
{
    ESP_LOGI(TAG, "GPIO %d pressed state changed to %d", button_event.gpio_num, button_event.pressed);

    int64_t lastPressTimestamp = button_event_press_timestamps[button_event.gpio_num];
    switch (button_event.pressed)
    {
    case 0:
        if (lastPressTimestamp == 0)
            return;

        button_event_press_timestamps[button_event.gpio_num] = 0;
        int64_t pressTime = (button_event.timestamp - lastPressTimestamp) / 1000;
        ESP_LOGI(TAG, "GPIO %d was pressed for %d ms", button_event.gpio_num, pressTime);
        break;

    case 1:
        if (lastPressTimestamp != 0)
            return;
            
        button_event_press_timestamps[button_event.gpio_num] = button_event.timestamp;
        break;
    }
}

void task_button_event_queue_handler(void *pvParameter)
{
    button_event_queue = xQueueCreate(10, sizeof(button_event_t));
    if (button_event_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return;
    }

    button_event_t button_event;
    while (1)
    {
        if (xQueueReceive(button_event_queue, &button_event, portMAX_DELAY))
            button_event_handler(button_event);
    }
}