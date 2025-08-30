#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "include/task_button_event_queue_handler.h"
#include "include/task_respawn_counter_mode_handler.h"
#include "include/task_buzzer_event_queue_handler.h"

#define RESPAWN_BUTTON_LED_PIN GPIO_NUM_2
static const char *TAG = "task_respawn_counter_mode_handler";
static bool setup_mode = false;
static uint16_t respawn_tokens = 10;
static respawn_policy_t respawn_policies[10] = {};

void short_press(void)
{
    ESP_LOGI(TAG, "Short press");

    if (respawn_tokens == 0)
    {
        ESP_LOGI(TAG, "No respawn token left");

        BEEP_TYPE respawn_no_tokens = RESPAWN_NO_TOKENS;
        if (xQueueSend(buzzer_event_queue, &respawn_no_tokens, 0) != pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to send buzzer event to queue");
            return;
        }
        return;
    }

    respawn_tokens--;
    ESP_LOGI(TAG, "Respawn token decremented. Tokens: %lu", respawn_tokens);

    bool respawn_batch = true;
    for (uint8_t i = 0; i < 10; i++)
    {
        // TODO: Implement respawn policy sort
        respawn_policy_t respawn_policy = respawn_policies[i];

        if (respawn_policy.enabled == false)
            continue;

        if (respawn_tokens > respawn_policy.min)
            continue;

        if (respawn_tokens < respawn_policy.max)
            continue;

        ESP_LOGI(TAG, "Using respawn policy. Priority: %u. Min: %lu. Max: %lu. Batch size: %lu", respawn_policy.priority, respawn_policy.min, respawn_policy.max, respawn_policy.batch_size);

        respawn_batch = respawn_tokens % respawn_policy.batch_size == 0;
    }

    if (respawn_batch == true)
    {
        ESP_LOGI(TAG, "Batch respawn");

        BEEP_TYPE respawn_batch = RESPAWN_BATCH;
        if (xQueueSend(buzzer_event_queue, &respawn_batch, 0) != pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to send buzzer event to queue");
            return;
        }
    }
    else
    {
        ESP_LOGI(TAG, "Waiting for batch respawn");

        BEEP_TYPE respawn_token_decrement = RESPAWN_TOKEN_DECREMENT;
        if (xQueueSend(buzzer_event_queue, &respawn_token_decrement, 0) != pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to send buzzer event to queue");
            return;
        }
    }

    // TODO: Update matrix display with respawn_tokens
}

void enable_setup_mode(void)
{
    ESP_LOGI(TAG, "Setup mode enable");

    BEEP_TYPE respawn_setup_mode_enabled = RESPAWN_SETUP_MODE_ENABLED;
    if (xQueueSend(buzzer_event_queue, &respawn_setup_mode_enabled, 0) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send buzzer event to queue");
        return;
    }

    setup_mode = true;

    // TODO: Implement
}

void setup_mode_short_press(void)
{
    ESP_LOGI(TAG, "Setup mode short press");

    // TODO: Implement
}

void setup_mode_long_press(void)
{
    ESP_LOGI(TAG, "Setup mode long press");

    // TODO: Implement

    setup_mode = false;
}

void respawn_mode_button_event_handler(button_event_t button_event)
{
    ESP_LOGI(TAG, "Button event received. GPIO: %u. State: %u. Duration: %lli ms", button_event.gpio_num, button_event.state, button_event.duration / 1000);

    if (button_event.state != DEPRESSED)
        return;

    switch (setup_mode)
    {
    case false:
        // 1 sec
        if (button_event.duration < 1000000)
            short_press();

        // 10 sec
        if (button_event.duration > 10000000)
            enable_setup_mode();
        break;

    case true:
        // 1 sec
        if (button_event.duration < 1000000)
            setup_mode_short_press();

        // 5 sec
        if (button_event.duration > 5000000)
            setup_mode_long_press();
        break;
    }
}

void task_respawn_counter_mode_handler(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    bool respawn_button_led = false;
    gpio_set_direction(RESPAWN_BUTTON_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RESPAWN_BUTTON_LED_PIN, respawn_button_led);

    ESP_LOGI(TAG, "Task setup complete");

    while (1)
    {
        if (respawn_tokens == 0)
        {
            if (respawn_button_led != false)
            {
                respawn_button_led = !respawn_button_led;
                gpio_set_level(RESPAWN_BUTTON_LED_PIN, respawn_button_led);
            }

            // TODO: Find a more elegant way to do nothing, but not cancel task
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        respawn_button_led = !respawn_button_led;
        gpio_set_level(RESPAWN_BUTTON_LED_PIN, respawn_button_led);
        vTaskDelay(pdMS_TO_TICKS(setup_mode == true ? 250 : 1000));
    }
}