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
static uint16_t respawn_tokens = 100;
static respawn_policy_t respawn_policies[10] = {};

void short_press(void)
{
    ESP_LOGI(TAG, "Short press");

    if (respawn_tokens == 0)
    {
        BEEP_TYPE respawn_no_tokens = RESPAWN_NO_TOKENS;
        xQueueOverwrite(buzzer_event_queue, &respawn_no_tokens);
        return;
    }

    respawn_tokens--;

    bool respawn_batch = false;
    for (uint8_t i = 0; i < 10; i++)
    {
        respawn_policy_t respawn_policy = respawn_policies[i];

        if (respawn_policy.enabled == false)
            continue;

        if (respawn_tokens > respawn_policy.min)
            continue;

        if (respawn_tokens < respawn_policy.max)
            continue;

        respawn_batch = respawn_tokens % respawn_policy.batch_size == 0;
    }

    if (respawn_batch == true)
    {
        BEEP_TYPE respawn_batch = RESPAWN_BATCH;
        xQueueOverwrite(buzzer_event_queue, &respawn_batch);
    }
    else
    {
        BEEP_TYPE respawn_token_decrease = RESPAWN_TOKEN_DECREASE;
        xQueueOverwrite(buzzer_event_queue, &respawn_token_decrease);
    }

    // TODO: Update matrix display with respawn_tokens
}

void enable_setup_mode(void)
{
    ESP_LOGI(TAG, "Setup mode enable");

    BEEP_TYPE respawn_setup_mode_enabled = RESPAWN_SETUP_MODE_ENABLED;
    xQueueOverwrite(buzzer_event_queue, &respawn_setup_mode_enabled);

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
}

void respawn_mode_button_event_handler(button_event_t button_event)
{
    ESP_LOGI(TAG, "Button event received. GPIO: %d. State: %d. Duration: %d ms", button_event.gpio_num, button_event.state, button_event.durationMS);

    if (button_event.state != DEPRESSED)
        return;

    switch (setup_mode)
    {
    case false:
        if (button_event.durationMS < 1000)
            short_press();

        if (button_event.durationMS > 10000)
            enable_setup_mode();
        break;

    case true:
        if (button_event.durationMS < 1000)
            setup_mode_short_press();

        if (button_event.durationMS > 5000)
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