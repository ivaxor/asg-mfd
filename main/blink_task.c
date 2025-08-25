#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

// Используем пин GPIO 5 для светодиода на Lolin D32
#define LED_PIN GPIO_NUM_5

void blink_task_function(void *pvParameter)
{
    // Настраиваем пин GPIO на вывод
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (true) {
        // Включаем светодиод
        gpio_set_level(LED_PIN, 0);
        printf("LED is ON\n");
        
        // Задержка на 1 секунду
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Выключаем светодиод
        gpio_set_level(LED_PIN, 1);
        printf("LED is OFF\n");
        
        // Задержка на 1 секунду
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}