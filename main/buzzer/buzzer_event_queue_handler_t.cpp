#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "include/buzzer_event_queue_handler_t.h"

#define BUZZER_PIN GPIO_NUM_4

const char *buzzer_event_queue_handler_t::TAG = "buzzer_event_queue_handler_t";
QueueHandle_t buzzer_event_queue_handler_t::queue;

void buzzer_event_queue_handler_t::init()
{
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_PIN, 0);

    queue = xQueueCreate(10, sizeof(BUZZER_BEEP_TYPE));
    if (queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create buzzer event queue");
        return;
    }
}

void buzzer_event_queue_handler_t::add_to_queue(BUZZER_BEEP_TYPE beep_type)
{
    if (xQueueSend(queue, &beep_type, 0) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send buzzer event to queue");
        return;
    }
}

void buzzer_event_queue_handler_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    while (1)
    {
        BUZZER_BEEP_TYPE beep_type;
        xQueueReceive(queue, &beep_type, portMAX_DELAY);
    new_beep:
        ESP_LOGI(TAG, "Buzzer event received. Type: %u", beep_type);

        switch (beep_type)
        {
        case RESPAWN_NO_TOKENS:
            ESP_LOGI(TAG, "Playing RESPAWN_NO_TOKENS");
            gpio_set_level(BUZZER_PIN, 1);
            if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(2000)) == pdTRUE)
                goto new_beep;
            gpio_set_level(BUZZER_PIN, 0);
            break;

        case RESPAWN_TOKEN_DECREMENT:
            ESP_LOGI(TAG, "Playing RESPAWN_TOKEN_DECREMENT");
            gpio_set_level(BUZZER_PIN, 1);
            if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                goto new_beep;
            gpio_set_level(BUZZER_PIN, 0);
            break;

        case RESPAWN_BATCH:
            ESP_LOGI(TAG, "Playing RESPAWN_BATCH");
            gpio_set_level(BUZZER_PIN, 1);
            if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                goto new_beep;
            gpio_set_level(BUZZER_PIN, 0);
            if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(300)) == pdTRUE)
                goto new_beep;
            for (uint8_t i = 0; i < 2; i++)
            {
                gpio_set_level(BUZZER_PIN, 1);
                if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(100)) == pdTRUE)
                    goto new_beep;
                gpio_set_level(BUZZER_PIN, 0);
                if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(100)) == pdTRUE)
                    goto new_beep;
            }
            break;

        case SETUP_MODE:
            ESP_LOGI(TAG, "Playing SETUP_MODE");
            for (uint8_t i = 0; i < 6; i++)
            {
                gpio_set_level(BUZZER_PIN, 1);
                if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(100)) == pdTRUE)
                    goto new_beep;
                gpio_set_level(BUZZER_PIN, 0);
                if (xQueueReceive(queue, &beep_type, pdMS_TO_TICKS(100)) == pdTRUE)
                    goto new_beep;
            }
            break;

        default:
            ESP_LOGE(TAG, "Unknown beep type received");
            break;
        }
    }
}