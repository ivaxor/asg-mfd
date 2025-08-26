#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/task_blink.h"
#include "include/setup_webserver.h"

void app_main(void)
{
    setup_webserver();

    xTaskCreate(&task_blink, "task_blink", 2048, NULL, 5, NULL);
}