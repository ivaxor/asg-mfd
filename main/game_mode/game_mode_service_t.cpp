#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "include/game_mode_service_t.hpp"
#include "../respawn_counter/include/respawn_counter_service_t.hpp"
#include "../matrix_display/include/matrix_display_service_t.hpp"
#include "../sd_card/include/sd_card_service_t.hpp"
#include "../led_strip/include/led_strip_service_t.hpp"

const char *game_mode_service_t::TAG = "game_mode_service_t";
game_mode_info_t game_mode_service_t::info;
TaskHandle_t game_mode_service_t::respawn_counter_task;
TaskHandle_t game_mode_service_t::led_strip_task;

void game_mode_service_t::init()
{
    ESP_LOGI(TAG, "Initializing");

    // TODO: Implement reading game mode from SD card
    GAME_MODE default_game_mode = RESPAWN_COUNTER;
    init_new(default_game_mode);
}

void game_mode_service_t::uninit()
{
    switch (info.mode)
    {
    case RESPAWN_COUNTER:
        vTaskDelete(respawn_counter_task);
        respawn_counter_service_t::uninit();
        matrix_display_service_t::uninit();
        break;

    case FLAG:
        // TODO: Implement
        vTaskDelete(led_strip_task);
        break;

    case BOMB:
        break;
    }
}

void game_mode_service_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(60000));

        switch (info.mode)
        {
        case RESPAWN_COUNTER:
        {
            ESP_LOGI(TAG, "Saving respawn counter info to SD card");
            respawn_counter_info_t *info = respawn_counter_service_t::get();
            sd_card_service_t::write_respawn_counter_info(info);
            break;
        }

        case FLAG:
        {
            ESP_LOGI(TAG, "Saving flag info to SD card");
            // TODO: Implement
            break;
        }

        default:
        {
            // TODO: Throw exception
            break;
        }
        }
    }
}

void game_mode_service_t::init_new(GAME_MODE new_mode)
{
    switch (new_mode)
    {
    case RESPAWN_COUNTER:
    {
        matrix_display_service_t::init();

        respawn_counter_service_t::init();
        respawn_counter_info_t *info = sd_card_service_t::read_respawn_counter_info();
        if (info != NULL)
        {
            ESP_LOGI(TAG, "Replacing game mode to %u", new_mode);
            respawn_counter_service_t::replace(info);
            delete[] info->policies;
            delete info;
        }

        xTaskCreate(respawn_counter_service_t::task, "respawn_counter_service_t", 4096, NULL, 5, &respawn_counter_task);
        break;
    }

    case FLAG:
    {
        led_strip_service_t::init();
        xTaskCreate(led_strip_service_t::task, "led_strip_service_t", 4096, NULL, 5, &led_strip_task);
        // TODO: Implement
        break;
    }

    case BOMB:
    {
        break;
    }
    }

    info.mode = new_mode;
    info.start_timestamp = esp_timer_get_time();
}

game_mode_info_t *game_mode_service_t::get()
{
    return &info;
}

void game_mode_service_t::replace(GAME_MODE new_mode)
{
    ESP_LOGI(TAG, "Replacing game mode to %u", new_mode);

    uninit();
    init_new(new_mode);
}