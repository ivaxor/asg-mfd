#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "button_gpio.h"
#include "iot_button.h"
#include "include/respawn_counter_info_t.hpp"
#include "include/respawn_counter_service_t.hpp"
#include "../buzzer/include/buzzer_event_queue_handler_t.hpp"
#include "../matrix_display/include/matrix_display_service_t.hpp"
#include "../sd_card/include/sd_card_service_t.hpp"
#include "../battery/include/battery_service_handler_t.hpp"
#include "../battery/include/battery_state_type.hpp"

#define RESPAWN_BUTTON_LED_PIN GPIO_NUM_5
#define RESPAWN_BUTTON_SWITCH_PIN GPIO_NUM_1

const char *respawn_counter_service_t::TAG = "respawn_counter_service_t";
respawn_counter_info_t respawn_counter_service_t::info;
bool respawn_counter_service_t::blink;
bool respawn_counter_service_t::setup_mode;
RESPAWN_COUNTER_SETUP_MENU respawn_counter_service_t::setup_mode_menu;
respawn_counter_info_t respawn_counter_service_t::setup_mode_info;

void respawn_counter_service_t::init()
{
    ESP_LOGI(TAG, "Initializing");

    setup_mode = false;

    info.respawn_tokens = 111;
    info.current_respawn_tokens = 111;
    info.policies = new respawn_counter_policy_t[0];
    info.policies_length = 0;

    gpio_set_direction(RESPAWN_BUTTON_LED_PIN, GPIO_MODE_OUTPUT);

    button_config_t button_config = {
        .long_press_time = 5000,
        .short_press_time = 25,
    };

    button_gpio_config_t button_gpio_config = {
        .gpio_num = RESPAWN_BUTTON_SWITCH_PIN,
        .active_level = 0,
        .enable_power_save = false,
    };

    button_handle_t button_handle = NULL;
    ESP_ERROR_CHECK(iot_button_new_gpio_device(&button_config, &button_gpio_config, &button_handle));

    iot_button_register_cb(button_handle, BUTTON_SINGLE_CLICK, NULL, handle_single_click, NULL);
    iot_button_register_cb(button_handle, BUTTON_LONG_PRESS_START, NULL, handle_long_click, NULL);
}

void respawn_counter_service_t::uninit()
{
    gpio_set_level(RESPAWN_BUTTON_LED_PIN, 0);
}

void respawn_counter_service_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    render_info_on_matrix_display();

    while (true)
    {
        blink = !blink;

        if (setup_mode)
        {
            render_setup_on_matrix_display();
            render_info_on_matrix_display();
            gpio_set_level(RESPAWN_BUTTON_LED_PIN, blink);

            vTaskDelay(pdMS_TO_TICKS(250));
            continue;
        }

        render_info_on_matrix_display();
        gpio_set_level(RESPAWN_BUTTON_LED_PIN, info.current_respawn_tokens == 0 ? false : blink);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void respawn_counter_service_t::handle_single_click(void *arg, void *usr_data)
{
    if (setup_mode)
        setup_mode_short_press();
    else
        short_press();
}

void respawn_counter_service_t::handle_long_click(void *arg, void *usr_data)
{
    if (setup_mode)
        setup_mode_long_press();
    else
        long_press();
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
        if (info.policies[i].disabled == true)
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

    render_info_on_matrix_display();
}

void respawn_counter_service_t::long_press()
{
    ESP_LOGI(TAG, "Setup mode enable");

    setup_mode = true;
    setup_mode_menu = REVERT;
    setup_mode_info = {
        .respawn_tokens = info.respawn_tokens,
        .policies = info.policies,
        .policies_length = info.policies_length,
    };

    buzzer_event_queue_handler_t::add_to_queue(SETUP_MODE);
    render_setup_on_matrix_display();
    render_info_on_matrix_display();
}

void respawn_counter_service_t::setup_mode_short_press()
{
    ESP_LOGI(TAG, "Setup mode short press");

    switch (setup_mode_menu)
    {
    case REVERT:
        setup_mode_menu = TOKENS_INCREASE;
        render_setup_on_matrix_display();
        break;

    case TOKENS_INCREASE:
        setup_mode_menu = TOKENS_DECREASE;
        render_setup_on_matrix_display();
        break;

    case TOKENS_INCREASE_SELECTED:
        setup_mode_info.respawn_tokens++;
        render_info_on_matrix_display();
        break;

    case TOKENS_DECREASE:
        setup_mode_menu = SAVE;
        render_setup_on_matrix_display();
        break;

    case TOKENS_DECREASE_SELECTED:
        setup_mode_info.respawn_tokens--;
        render_info_on_matrix_display();
        break;

    case SAVE:
        setup_mode_menu = REVERT;
        render_setup_on_matrix_display();
        break;
    }

    buzzer_event_queue_handler_t::add_to_queue(RESPAWN_TOKEN_DECREMENT);
}

void respawn_counter_service_t::setup_mode_long_press()
{
    ESP_LOGI(TAG, "Setup mode long press");

    switch (setup_mode_menu)
    {
    case REVERT:
        setup_mode = false;
        info = {
            .respawn_tokens = info.respawn_tokens,
            .current_respawn_tokens = info.respawn_tokens,
            .policies = info.policies,
            .policies_length = info.policies_length,
        };
        buzzer_event_queue_handler_t::add_to_queue(SETUP_MODE);
        render_setup_on_matrix_display();
        render_info_on_matrix_display();
        sd_card_service_t::write_respawn_counter_info(&info);
        break;

    case TOKENS_INCREASE:
        setup_mode_menu = TOKENS_INCREASE_SELECTED;
        buzzer_event_queue_handler_t::add_to_queue(RESPAWN_BATCH);
        render_setup_on_matrix_display();
        break;

    case TOKENS_INCREASE_SELECTED:
        setup_mode_menu = TOKENS_INCREASE;
        buzzer_event_queue_handler_t::add_to_queue(RESPAWN_BATCH);
        render_setup_on_matrix_display();
        break;

    case TOKENS_DECREASE:
        setup_mode_menu = TOKENS_DECREASE_SELECTED;
        buzzer_event_queue_handler_t::add_to_queue(RESPAWN_BATCH);
        render_setup_on_matrix_display();
        break;

    case TOKENS_DECREASE_SELECTED:
        setup_mode_menu = TOKENS_DECREASE;
        buzzer_event_queue_handler_t::add_to_queue(RESPAWN_BATCH);
        render_setup_on_matrix_display();
        break;

    case SAVE:
        setup_mode = false;
        info = {
            .respawn_tokens = setup_mode_info.respawn_tokens,
            .current_respawn_tokens = setup_mode_info.respawn_tokens,
            .policies = setup_mode_info.policies,
            .policies_length = setup_mode_info.policies_length,
        };
        buzzer_event_queue_handler_t::add_to_queue(SETUP_MODE);
        render_setup_on_matrix_display();
        render_info_on_matrix_display();
        sd_card_service_t::write_respawn_counter_info(&info);
        break;
    }
}

void respawn_counter_service_t::render_setup_on_matrix_display()
{
    if (setup_mode == false)
    {
        matrix_display_service_t::clear(0);
        matrix_display_service_t::clear(4);
        return;
    }

    BATTERY_STATE_TYPE battery_state = battery_service_handler_t::state();

    switch (setup_mode_menu)
    {
    case REVERT:
        matrix_display_service_t::draw_special_character(0, WRENCH);
        if (blink)
            matrix_display_service_t::draw_special_character(4, REWIND);
        else if (battery_state == LOW || battery_state == UNKNOWN)
            matrix_display_service_t::draw_special_character(4, BATTERY);
        else
            matrix_display_service_t::clear(4);
        break;

    case TOKENS_INCREASE:
        matrix_display_service_t::draw_special_character(0, WRENCH);
        if (blink)
            matrix_display_service_t::draw_special_character(4, ARROW_UP_RIGHT);
        else if (battery_state == LOW || battery_state == UNKNOWN)
            matrix_display_service_t::draw_special_character(4, BATTERY);
        else
            matrix_display_service_t::clear(4);
        break;

    case TOKENS_INCREASE_SELECTED:
        matrix_display_service_t::draw_special_character(0, WRENCH);
        if (blink)
            matrix_display_service_t::draw_special_character(4, ARROW_UP_RIGHT);
        else if (battery_state == LOW || battery_state == UNKNOWN)
            matrix_display_service_t::draw_special_character(4, BATTERY);
        break;

    case TOKENS_DECREASE:
        matrix_display_service_t::draw_special_character(0, WRENCH);
        if (blink)
            matrix_display_service_t::draw_special_character(4, ARROW_DOWN_RIGHT);
        else if (battery_state == LOW || battery_state == UNKNOWN)
            matrix_display_service_t::draw_special_character(4, BATTERY);
        else
            matrix_display_service_t::clear(4);
        break;

    case TOKENS_DECREASE_SELECTED:
        matrix_display_service_t::draw_special_character(0, WRENCH);
        if (blink)
            matrix_display_service_t::draw_special_character(4, ARROW_DOWN_RIGHT);
        else if (battery_state == LOW || battery_state == UNKNOWN)
            matrix_display_service_t::draw_special_character(4, BATTERY);
        break;

    case SAVE:
        matrix_display_service_t::draw_special_character(0, WRENCH);
        if (blink)
            matrix_display_service_t::draw_special_character(4, CHECK_MARK);
        else if (battery_state == LOW || battery_state == UNKNOWN)
            matrix_display_service_t::draw_special_character(4, BATTERY);
        else
            matrix_display_service_t::clear(4);
        break;
    }
}

void respawn_counter_service_t::render_info_on_matrix_display()
{
    if (setup_mode == false)
    {
        BATTERY_STATE_TYPE battery_state = battery_service_handler_t::state();
        if ((battery_state == LOW || battery_state == UNKNOWN) && !blink)
            matrix_display_service_t::draw_special_character(4, BATTERY);
        else
            matrix_display_service_t::clear(4);

        matrix_display_service_t::draw_tall_number(1, 5, info.current_respawn_tokens / 100);
        matrix_display_service_t::draw_tall_number(2, 6, (info.current_respawn_tokens / 10) % 10);
        matrix_display_service_t::draw_tall_number(3, 7, info.current_respawn_tokens % 10);
        return;
    }

    if (blink == false && (setup_mode_menu == TOKENS_INCREASE_SELECTED || setup_mode_menu == TOKENS_DECREASE_SELECTED))
    {
        matrix_display_service_t::clear(1);
        matrix_display_service_t::clear(5);

        matrix_display_service_t::clear(2);
        matrix_display_service_t::clear(6);

        matrix_display_service_t::clear(3);
        matrix_display_service_t::clear(7);
        return;
    }

    matrix_display_service_t::draw_tall_number(1, 5, setup_mode_info.respawn_tokens / 100);
    matrix_display_service_t::draw_tall_number(2, 6, (setup_mode_info.respawn_tokens / 10) % 10);
    matrix_display_service_t::draw_tall_number(3, 7, setup_mode_info.respawn_tokens % 10);
}

respawn_counter_info_t *respawn_counter_service_t::get()
{
    return &info;
}

void respawn_counter_service_t::replace(respawn_counter_info_t *new_info)
{
    delete[] info.policies;

    info = {
        .respawn_tokens = new_info->respawn_tokens,
        .current_respawn_tokens = new_info->current_respawn_tokens,
        .policies = new respawn_counter_policy_t[new_info->policies_length],
        .policies_length = new_info->policies_length,
    };

    for (uint8_t i = 0; i < new_info->policies_length; i++)
    {
        info.policies[i] = {
            .priority = new_info->policies[i].priority,
            .disabled = new_info->policies[i].disabled,
            .min = new_info->policies[i].min,
            .max = new_info->policies[i].max,
            .batch_size = new_info->policies[i].batch_size,
        };
    }
}