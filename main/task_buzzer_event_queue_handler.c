#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "include/task_buzzer_event_queue_handler.h"

static const char *TAG = "task_buzzer_event_queue_handler";

QueueHandle_t buzzer_event_queue;

#define BUZZER_PIN GPIO_NUM_7
void task_buzzer_event_queue_handler(void *pvParameter)
{
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_PIN, 0);

    buzzer_event_queue = xQueueCreate(2, sizeof(BEEP_TYPE));
    if (buzzer_event_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create buzzer event queue");
        return;
    }

    BEEP_TYPE beep_type;
    while (1)
    {
        xQueueReceive(buzzer_event_queue, &beep_type, portMAX_DELAY);

    new_beep:
        switch (beep_type)
        {
        case RESPAWN_NO_TOKENS:
            gpio_set_level(BUZZER_PIN, 1);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(2000)) == pdTRUE)
                goto new_beep;
            gpio_set_level(BUZZER_PIN, 0);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(2000)) == pdTRUE)
                goto new_beep;
            break;

        case RESPAWN_TOKEN_DECREASE:
            gpio_set_level(BUZZER_PIN, 1);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                goto new_beep;
            gpio_set_level(BUZZER_PIN, 0);
            if (xQueueReceive(buzzer_event_queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                goto new_beep;
            break;

        case RESPAWN_BATCH:
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