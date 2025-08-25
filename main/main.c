#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "blink_task.h"

void app_main(void)
{
    // Создаём задачу для мигания.
    // Приоритет 5 является стандартным для большинства пользовательских задач.
    xTaskCreate(&blink_task_function, "blink_task", 2048, NULL, 5, NULL);
}