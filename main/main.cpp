#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "button/include/button_event_queue_handler_t.h"
#include "button/include/button_isr_handler_t.h"
#include "button/include/button_raw_event_queue_handler_t.h"
#include "buzzer/include/buzzer_event_queue_handler_t.h"
#include "game_mode/include/game_mode_service_t.h"
#include "led/include/led_heartbeat_service_t.h"
#include "webserver/include/webserver_service_t.h"

static const char *TAG = "app_main";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting application setup");
    button_event_queue_handler_t::init();
    button_isr_handler_t::init();
    button_raw_event_queue_handler_t::init();
    buzzer_event_queue_handler_t::init();
    game_mode_service_t::init();
    led_heartbeat_service_t::init();
    webserver_service_t::init();
    ESP_LOGI(TAG, "Application setup completed");

    ESP_LOGI(TAG, "Starting application tasks");
    xTaskCreate(button_event_queue_handler_t::task, "button_event_queue_handler_t", 4096, NULL, 5, NULL);
    xTaskCreate(button_raw_event_queue_handler_t::task, "button_raw_event_queue_handler_t", 4096, NULL, 5, NULL);
    xTaskCreate(buzzer_event_queue_handler_t::task, "buzzer_event_queue_handler_t", 4096, NULL, 5, NULL);
    xTaskCreate(led_heartbeat_service_t::task, "led_heartbeat_service_t", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Application tasks started");
}