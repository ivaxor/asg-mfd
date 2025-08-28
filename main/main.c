#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "include/setup_button_isr.h"
#include "include/setup_webserver.h"
#include "include/task_blink.h"
#include "include/task_button_event_queue_handler.h"

void app_main(void)
{
    setup_webserver();
    setup_button_isr();

    xTaskCreate(&task_blink, "task_blink", 2048, NULL, 5, NULL);
    xTaskCreate(&task_button_event_queue_handler, "task_button_event_queue_handler", 2048, NULL, 5, NULL);
}