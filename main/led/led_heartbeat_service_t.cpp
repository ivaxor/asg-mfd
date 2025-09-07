#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "include/led_heartbeat_service_t.h"

const char *led_heartbeat_service_t::TAG = "led_heartbeat_service_t";
led_strip_handle_t led_heartbeat_service_t::led_strip;

void led_heartbeat_service_t::init()
{
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000,
        .flags = {
            .with_dma = false,
        },
    };

    led_strip_config_t strip_config = {
        .strip_gpio_num = GPIO_NUM_38,
        .max_leds = 1,
        .flags = {
            .invert_out = false,
        },
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

void led_heartbeat_service_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    bool fade = false;
    uint8_t intensity = 0;

    while (1)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 0, intensity, 0));
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));

        vTaskDelay(pdMS_TO_TICKS(50));

        if (intensity == 255)
            fade = true;

        if (intensity == 0)
            fade = false;

        if (fade == false)
            intensity += 5;
        else
            intensity -= 5;
    }
}