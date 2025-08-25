#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_blink.h"
#include "setup_webserver.h"
#include "setup_wifi.h"

void app_main(void)
{
    setup_wifi();
    setup_webserver();

    xTaskCreate(&task_blink, "task_blink", 2048, NULL, 5, NULL);
}