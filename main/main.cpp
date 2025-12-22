#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "buzzer/include/buzzer_event_queue_handler_t.hpp"
#include "game_mode/include/game_mode_service_t.hpp"
#include "led/include/led_heartbeat_service_t.hpp"
#include "sd_card/include/sd_card_service_t.hpp"
#include "spi/include/spi_service_t.hpp"
#include "webserver/include/webserver_service_t.hpp"

static const char *TAG = "app_main";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting application setup");
    spi_service_t::init();
    sd_card_service_t::init();
    led_heartbeat_service_t::init();
    webserver_service_t::init();
    buzzer_event_queue_handler_t::init();
    game_mode_service_t::init();
    ESP_LOGI(TAG, "Application setup completed");

    ESP_LOGI(TAG, "Starting application tasks");
    xTaskCreate(buzzer_event_queue_handler_t::task, "buzzer_event_queue_handler_t", 4096, NULL, 5, NULL);
    xTaskCreate(game_mode_service_t::task, "game_mode_service_t", 4096, NULL, 5, NULL);
    xTaskCreate(led_heartbeat_service_t::task, "led_heartbeat_service_t", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Application tasks started");

    buzzer_event_queue_handler_t::add_to_queue(BUZZER_BEEP_TYPE::SETUP_MODE);
}