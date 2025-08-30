#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "include/task_buzzer_event_queue_handler.h"

static const char *TAG = "task_buzzer_event_queue_handler";

#define BUZZER_PIN GPIO_NUM_4

QueueHandle_t buzzer_event_queue;

void task_buzzer_event_queue_handler(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_PIN, 0);

    buzzer_event_queue = xQueueCreate(10, sizeof(BEEP_TYPE));
    if (buzzer_event_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create buzzer event queue");
        return;
    }

    ESP_LOGI(TAG, "Task setup complete");

    BEEP_TYPE beep_type;
    while (1)
    {
        xQueueReceive(buzzer_event_queue, &beep_type, portMAX_DELAY);
    new_beep:
        ESP_LOGI(TAG, "Buzzer event received. Type: %d", beep_type);

        switch (beep_type)
        {
        case RESPAWN_NO_TOKENS:
            ESP_LOGI(TAG, "Playing RESPAWN_NO_TOKENS");
            gpio_set_level(BUZZER_PIN, 1);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(2000)) == pdTRUE)
                goto new_beep;
            gpio_set_level(BUZZER_PIN, 0);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(2000)) == pdTRUE)
                goto new_beep;
            break;

        case RESPAWN_TOKEN_DECREASE:
            ESP_LOGI(TAG, "Playing RESPAWN_TOKEN_DECREASE");
            gpio_set_level(BUZZER_PIN, 1);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                goto new_beep;
            gpio_set_level(BUZZER_PIN, 0);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                goto new_beep;
            break;

        case RESPAWN_BATCH:
            ESP_LOGI(TAG, "Playing RESPAWN_BATCH");
            for (uint8_t i = 0; i < 3; i++)
            {
                gpio_set_level(BUZZER_PIN, 1);
                if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                    goto new_beep;
                gpio_set_level(BUZZER_PIN, 0);
                if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                    goto new_beep;
            }
            break;

        case RESPAWN_SETUP_MODE_ENABLED:
            ESP_LOGI(TAG, "Playing RESPAWN_SETUP_MODE_ENABLED");
            for (uint8_t i = 0; i < 3; i++)
            {
                gpio_set_level(BUZZER_PIN, 1);
                if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300 - (100 * i))) == pdTRUE)
                    goto new_beep;
                gpio_set_level(BUZZER_PIN, 0);
                if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300 - (100 * i))) == pdTRUE)
                    goto new_beep;
            }
            break;

        default:
            ESP_LOGE(TAG, "Unknown beep type received");
            break;
        }
    }
}