#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "include/game_mode_service_t.h"
#include "../respawn_counter/include/respawn_counter_service_t.h"
#include "../matrix_display/include/matrix_display_service_t.h"

const char *game_mode_service_t::TAG = "game_mode_service_t";
game_mode_info_t game_mode_service_t::info;
TaskHandle_t game_mode_service_t::respawn_counter_task;

void game_mode_service_t::init()
{
    GAME_MODE default_game_mode = RESPAWN_COUNTER;
    replace_init_new(default_game_mode);
}

void game_mode_service_t::replace_cleanup()
{
    switch (info.mode)
    {
    case RESPAWN_COUNTER:
        vTaskDelete(respawn_counter_task);
        respawn_counter_service_t::uninit();
        matrix_display_service_t::uninit();
        break;

    case BOMB:
        // TODO: Implement
        break;

    case FLAG:
        // TODO: Implement
        break;
    }
}

void game_mode_service_t::replace_init_new(GAME_MODE new_mode)
{
    switch (new_mode)
    {
    case RESPAWN_COUNTER:
        matrix_display_service_t::init();
        respawn_counter_service_t::init();
        xTaskCreate(respawn_counter_service_t::task, "respawn_counter_service_t", 4096, NULL, 5, &respawn_counter_task);
        break;

    case BOMB:
        // TODO: Implement
        break;

    case FLAG:
        // TODO: Implement
        break;
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
    ESP_LOGI(TAG, "Replace game mode to %u", new_mode);

    replace_cleanup();
    replace_init_new(new_mode);
}