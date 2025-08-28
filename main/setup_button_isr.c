#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "include/task_button_event_queue_handler.h"

#define BUTTON_GPIO GPIO_NUM_0

void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;

    button_event_t event;

    event.gpio_num = gpio_num;
    event.high = gpio_get_level(gpio_num);

    xQueueSendFromISR(button_event_queue, &event, NULL);
}

void setup_button_isr(void)
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
    };
    ESP_ERROR_CHECK(gpio_config(&io_config));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, (void *)BUTTON_GPIO));
}