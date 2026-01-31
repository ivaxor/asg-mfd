#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "include/led_strip_service_t.hpp"

#define LED_STRIP_PIN GPIO_NUM_41
#define LED_STRIP_LEDS 16

const char *led_strip_service_t::TAG = "led_strip_service_t";
const uint8_t led_strip_service_t::MAX_COLOR_VALUE = 255;
const uint16_t led_strip_service_t::CYCLE_LENGTH = 6 * MAX_COLOR_VALUE;
led_strip_handle_t led_strip_service_t::led_strip;

void led_strip_service_t::init()
{
    ESP_LOGI(TAG, "Initializing");

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000,
        .flags = {
            .with_dma = true,
        },
    };

    led_strip_config_t led_strip_config = {
        .strip_gpio_num = LED_STRIP_PIN,
        .max_leds = LED_STRIP_LEDS,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB,
        .flags = {
            .invert_out = false,
        },
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

void led_strip_service_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    uint16_t step = 0;

    while (1)
    {
        for (uint8_t led = 0; led < LED_STRIP_LEDS; led++)
        {
            ESP_ERROR_CHECK(led_strip_set_pixel(
                led_strip,
                led,
                calculate_r(step + led * 4),
                calculate_g(step + led * 4),
                calculate_b(step + led * 4)));
        }
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));

        vTaskDelay(pdMS_TO_TICKS(10));

        step += 1;
    }
}

uint8_t led_strip_service_t::calculate_r(uint16_t step)
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

uint8_t led_strip_service_t::calculate_g(uint16_t step)
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

uint8_t led_strip_service_t::calculate_b(uint16_t step)
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