#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "battery/include/battery_state_type.hpp"
#include "battery/include/battery_service_handler_t.hpp"
#include "buzzer/include/buzzer_event_queue_handler_t.hpp"
#include "game_mode/include/game_mode_service_t.hpp"
#include "led_heartbeat/include/led_heartbeat_service_t.hpp"
#include "sd_card/include/sd_card_service_t.hpp"
#include "spi/include/spi_service_t.hpp"
#include "webserver/include/webserver_service_t.hpp"

static const char *TAG = "app_main";

extern "C" void app_main(void)
{
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = 0xFFFFFFFF,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    ESP_LOGI(TAG, "Starting application setup");
    spi_service_t::init();
    sd_card_service_t::init();
    // TODO: Remove after issue resovled:
    /*
I (809) led_strip_service_t: Initializing
E (819) rmt: rmt_tx_register_to_group(152): no free tx channels
E (819) rmt: rmt_new_tx_channel(286): register channel failed
E (829) led_strip_rmt: led_strip_new_rmt_device(154): create RMT TX channel failed
    */
    led_heartbeat_service_t::init();
    webserver_service_t::init();
    buzzer_event_queue_handler_t::init();
    battery_service_handler_t::init();
    game_mode_service_t::init();
    ESP_LOGI(TAG, "Application setup completed");

    BATTERY_STATE_TYPE battery_state = battery_service_handler_t::state();
    switch (battery_state)
    {
    case NORMAL:
    case LOW:
    case UNKNOWN:
        ESP_LOGI(TAG, "Starting application tasks");
        xTaskCreate(buzzer_event_queue_handler_t::task, "buzzer_event_queue_handler_t", 4096, NULL, 5, NULL);
        xTaskCreate(battery_service_handler_t::task, "battery_service_handler_t", 4096, NULL, 5, NULL);
        xTaskCreate(game_mode_service_t::task, "game_mode_service_t", 4096, NULL, 5, NULL);
        // TODO: Remove after issue resolved
        xTaskCreate(led_heartbeat_service_t::task, "led_heartbeat_service_t", 4096, NULL, 5, NULL);
        ESP_LOGI(TAG, "Application tasks started");

        buzzer_event_queue_handler_t::add_to_queue(BUZZER_BEEP_TYPE::SETUP_MODE);
        break;

    case CUTOFF:
        ESP_LOGI(TAG, "Battery is critically low. Cutting off");

        buzzer_event_queue_handler_t::add_to_queue(BUZZER_BEEP_TYPE::LOW_BATTERY);
        vTaskDelay(pdMS_TO_TICKS(5000));

        gpio_config(&gpio_cfg);
        esp_deep_sleep_start();
        break;
    }
}