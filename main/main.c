#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/task_blink.h"
#include "include/setup_webserver.h"
#include "include/setup_webserver2.h"
#include "include/setup_wifi.h"

void app_main(void)
{
    setup_webserver2();

    xTaskCreate(&task_blink, "task_blink", 2048, NULL, 5, NULL);
}