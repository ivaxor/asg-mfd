#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "include/task_button_event_queue_handler.h"
#include "include/task_respawn_counter_mode_handler.h"

static const char *TAG = "task_button_event_queue_handler";

QueueHandle_t button_event_queue;
void task_button_event_queue_handler(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    button_event_queue = xQueueCreate(10, sizeof(button_event_t));
    if (button_event_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return;
    }

    ESP_LOGI(TAG, "Task setup complete");

    button_event_t button_event;
    while (1)
    {
        xQueueReceive(button_event_queue, &button_event, portMAX_DELAY);
        ESP_LOGI(TAG, "Button event received. GPIO %d. Pressed: %s. Duration: %d ms", button_event.gpio_num, button_event.state, button_event.durationMS);

        // TODO: Add mode select
        respawn_mode_button_event_handler(button_event);
    }
}