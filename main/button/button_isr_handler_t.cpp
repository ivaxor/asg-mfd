#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "include/button_isr_handler_t.h"
#include "include/button_raw_event_queue_handler_t.h"

#define RESPAWN_BUTTON_PIN GPIO_NUM_0

void IRAM_ATTR button_isr_handler_t::handler(void *arg)
{
    gpio_num_t gpio_num = static_cast<gpio_num_t>(reinterpret_cast<int>(arg));

    button_raw_event_t button_raw_event;

    button_raw_event.gpio_num = gpio_num;
    button_raw_event.pressed = !gpio_get_level(gpio_num);
    button_raw_event.timestamp = esp_timer_get_time();

    button_raw_event_queue_handler_t::add_to_queue_isr(&button_raw_event);
}

void button_isr_handler_t::init()
{
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << RESPAWN_BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_config));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(RESPAWN_BUTTON_PIN, button_isr_handler_t::handler, (void *)RESPAWN_BUTTON_PIN));
}