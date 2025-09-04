#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "include/respawn_counter_info_t.h"
#include "include/respawn_counter_service_t.h"
#include "../buzzer/include/buzzer_event_queue_handler_t.h"
#include "../matrix_display/include/matrix_display_service_t.h"

#define RESPAWN_BUTTON_LED_PIN GPIO_NUM_2

const char *respawn_counter_service_t::TAG = "respawn_counter_service_t";
bool respawn_counter_service_t::setup_mode;
uint8_t respawn_counter_service_t::setup_mode_menu;
respawn_counter_info_t respawn_counter_service_t::info;

void respawn_counter_service_t::init()
{
    setup_mode = false;

    info.respawn_tokens = 0;
    info.current_respawn_tokens = 111;
    info.policies = new respawn_counter_policy_t[0];
    info.policies_length = 0;

    gpio_set_direction(RESPAWN_BUTTON_LED_PIN, GPIO_MODE_OUTPUT);
}

void respawn_counter_service_t::uninit()
{
    gpio_set_level(RESPAWN_BUTTON_LED_PIN, 0);
}

void respawn_counter_service_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    draw_on_matrix_display();

    bool respawn_button_led = false;

    while (1)
    {
        if (info.current_respawn_tokens == 0)
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

void respawn_counter_service_t::handle_button_event(button_event_t button_event)
{
    ESP_LOGI(TAG, "Button event received. GPIO: %u. State: %u. Duration: %lli ms", button_event.gpio_num, button_event.state, button_event.duration / 1000);

    if (button_event.state != DEPRESSED)
        return;

    switch (setup_mode)
    {
    case false:
        // 5 sec
        if (button_event.duration <= 5000000)
            short_press();
        else
            long_press();
        break;

    case true:
        // 5 sec
        if (button_event.duration <= 5000000)
            setup_mode_short_press();
        else
            setup_mode_long_press();
        break;
    }
}

void respawn_counter_service_t::short_press()
{
    ESP_LOGI(TAG, "Short press");

    if (info.current_respawn_tokens == 0)
    {
        ESP_LOGI(TAG, "No respawn token left");
        buzzer_event_queue_handler_t::add_to_queue(RESPAWN_NO_TOKENS);
        return;
    }

    info.current_respawn_tokens--;
    ESP_LOGI(TAG, "Respawn token decremented. Tokens: %lu", info.current_respawn_tokens);

    bool respawn_batch = true;

    for (uint8_t i = 0; i < info.policies_length; i++)
    {
        if (info.policies[i].enabled == false)
            continue;

        if (info.current_respawn_tokens > info.policies[i].min)
            continue;

        if (info.current_respawn_tokens < info.policies[i].max)
            continue;

        ESP_LOGI(TAG, "Using respawn policy. Priority: %u. Min: %lu. Max: %lu. Batch size: %lu", info.policies[i].priority, info.policies[i].min, info.policies[i].max, info.policies[i].batch_size);

        respawn_batch = info.current_respawn_tokens % info.policies[i].batch_size == 0;
    }

    if (respawn_batch == true)
    {
        ESP_LOGI(TAG, "Batch respawn");
        buzzer_event_queue_handler_t::add_to_queue(RESPAWN_BATCH);
    }
    else
    {
        ESP_LOGI(TAG, "Waiting for batch respawn");
        buzzer_event_queue_handler_t::add_to_queue(RESPAWN_TOKEN_DECREMENT);
    }

    draw_on_matrix_display();
}

void respawn_counter_service_t::long_press()
{
    ESP_LOGI(TAG, "Setup mode enable");

    setup_mode = true;
    setup_mode_menu = 0;
    buzzer_event_queue_handler_t::add_to_queue(SETUP_MODE_ENABLED);
    draw_on_matrix_display();

    // TODO: Implement
}

void respawn_counter_service_t::setup_mode_short_press()
{
    ESP_LOGI(TAG, "Setup mode short press");

    if (setup_mode_menu == 2)
        setup_mode_menu = 0;
    else
        setup_mode_menu++;

    draw_on_matrix_display();

    // TODO: Implement
}

void respawn_counter_service_t::setup_mode_long_press()
{
    ESP_LOGI(TAG, "Setup mode long press");

    setup_mode = false;
    buzzer_event_queue_handler_t::add_to_queue(SETUP_MODE_DISABLED);
    draw_on_matrix_display();

    // TODO: Implement
}

void respawn_counter_service_t::draw_on_matrix_display()
{
    matrix_display_service_t::clear_all();

    if (setup_mode == true)
    {
        matrix_display_service_t::draw_special_character(0, WRENCH);

        switch (setup_mode_menu)
        {
        case 0:
            matrix_display_service_t::draw_special_character(4, GROUP);
            break;

        case 1:
            matrix_display_service_t::draw_special_character(4, ARROW_UP_RIGHT);
            break;

        case 2:
            matrix_display_service_t::draw_special_character(4, ARROW_DOWN_RIGHT);
            break;
        }
    }

    matrix_display_service_t::draw_tall_number(1, 5, info.current_respawn_tokens / 100);
    matrix_display_service_t::draw_tall_number(2, 6, (info.current_respawn_tokens / 10) % 10);
    matrix_display_service_t::draw_tall_number(3, 7, info.current_respawn_tokens % 10);
}

respawn_counter_info_t respawn_counter_service_t::get()
{
    return info;
}

void respawn_counter_service_t::replace(respawn_counter_info_t new_info)
{
    delete[] info.policies;
    info = new_info;
}