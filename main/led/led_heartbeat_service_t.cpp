#include "driver/gpio.h"
#include "esp_log.h"
#include "include/led_heartbeat_service_t.h"

led_heartbeat_service_t led_heartbeat_service;

void led_heartbeat_service_t::init()
{
    gpio_set_direction(LED_HEARTBEAT_PIN, GPIO_MODE_OUTPUT);
}

void led_heartbeat_service_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    while (1)
    {
        gpio_set_level(LED_HEARTBEAT_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        gpio_set_level(LED_HEARTBEAT_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}