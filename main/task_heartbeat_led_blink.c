#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define LED_PIN GPIO_NUM_5

static const char *TAG = "task_heartbeat_led_blink";

void task_heartbeat_led_blink(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    
    ESP_LOGI(TAG, "Task setup complete");

    while (1)
    {
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}