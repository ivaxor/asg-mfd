#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "include/led_heartbeat_service_t.h"

const char *led_heartbeat_service_t::TAG = "led_heartbeat_service_t";
const uint8_t led_heartbeat_service_t::MAX_COLOR_VALUE = 255;
const uint16_t led_heartbeat_service_t::CYCLE_LENGTH = 6 * MAX_COLOR_VALUE;
led_strip_handle_t led_heartbeat_service_t::led;
led_strip_handle_t led_heartbeat_service_t::led_strip;

void led_heartbeat_service_t::init()
{
    ESP_LOGI(TAG, "Initializing");

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000,
        .flags = {
            .with_dma = true,
        },
    };

    led_strip_config_t led_config = {
        .strip_gpio_num = GPIO_NUM_38,
        .max_leds = 1,
        .flags = {
            .invert_out = false,
        },
    };

    led_strip_config_t led_strip_config = {
        .strip_gpio_num = GPIO_NUM_40,
        .max_leds = 60,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB,
        .flags = {
            .invert_out = false,
        },
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_config, &rmt_config, &led));
    //ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_strip_config, &rmt_config, &led_strip));

    ESP_ERROR_CHECK(led_strip_clear(led));
    //ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

void led_heartbeat_service_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    bool fade = false;
    uint8_t intensity = 0;

    //uint16_t step = 0;

    while (1)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led, 0, 0, intensity, 0));
        ESP_ERROR_CHECK(led_strip_refresh(led));

        /*
        for (uint8_t led = 0; led < 60; led++)
        {
            ESP_ERROR_CHECK(led_strip_set_pixel(
                led_strip,
                led,
                calculate_r(step + led * 16),
                calculate_g(step + led * 16),
                calculate_b(step + led * 16)));
        }
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        */

        vTaskDelay(pdMS_TO_TICKS(50));

        if (intensity == 255)
            fade = true;

        if (intensity == 0)
            fade = false;

        if (fade == false)
            intensity += 5;
        else
            intensity -= 5;

        //step++;
    }
}

uint8_t led_heartbeat_service_t::calculate_r(uint16_t step)
{
    step %= CYCLE_LENGTH;

    if (step < MAX_COLOR_VALUE)
        return MAX_COLOR_VALUE;
    if (step < 2 * MAX_COLOR_VALUE)
        return MAX_COLOR_VALUE - (step - MAX_COLOR_VALUE);
    if (step < 4 * MAX_COLOR_VALUE)
        return 0;
    if (step < 5 * MAX_COLOR_VALUE)
        return step - (4 * MAX_COLOR_VALUE);

    return MAX_COLOR_VALUE;
}

uint8_t led_heartbeat_service_t::calculate_g(uint16_t step)
{
    step %= CYCLE_LENGTH;

    if (step < MAX_COLOR_VALUE)
        return step;
    if (step < 4 * MAX_COLOR_VALUE)

    {
        if (step < 3 * MAX_COLOR_VALUE)
            return MAX_COLOR_VALUE;
        return MAX_COLOR_VALUE - (step - (3 * MAX_COLOR_VALUE));
    }

    return 0;
}

uint8_t led_heartbeat_service_t::calculate_b(uint16_t step)
{
    step %= CYCLE_LENGTH;

    if (step < 2 * MAX_COLOR_VALUE)
        return 0;
    if (step < 3 * MAX_COLOR_VALUE)
        return step - (2 * MAX_COLOR_VALUE);
    if (step < 5 * MAX_COLOR_VALUE)
        return MAX_COLOR_VALUE;

    return MAX_COLOR_VALUE - (step - (5 * MAX_COLOR_VALUE));
}