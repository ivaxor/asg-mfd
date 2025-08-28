#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "include/task_button_event_queue_handler.h"

static const char *TAG = "task_button_event_queue_handler";

QueueHandle_t button_event_queue;

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
        {
            ESP_LOGI(TAG, "GPIO %d state changed to %d", button_event.gpio_num, button_event.high);
        }
    }
}