#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "include/setup_button_isr.h"
#include "include/setup_webserver.h"
#include "include/task_button_event_queue_handler.h"
#include "include/task_button_raw_event_queue_handler.h"
#include "include/task_buzzer_event_queue_handler.h"
#include "include/task_heartbeat_led_blink.h"
#include "include/task_respawn_counter_mode_handler.h"

static const char *TAG = "app_main";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting application");
    setup_webserver();
    setup_button_isr();
    ESP_LOGI(TAG, "Application setup completed");

    ESP_LOGI(TAG, "Starting application tasks");
    xTaskCreate(&task_button_event_queue_handler, "task_button_event_queue_handler", 2048, NULL, 5, NULL);
    xTaskCreate(&task_button_raw_event_queue_handler, "task_button_raw_event_queue_handler", 2048, NULL, 5, NULL);
    xTaskCreate(&task_buzzer_event_queue_handler, "task_buzzer_event_queue_handler", 2048, NULL, 5, NULL);
    xTaskCreate(&task_heartbeat_led_blink, "task_heartbeat_led_blink", 2048, NULL, 5, NULL);
    xTaskCreate(&task_respawn_counter_mode_handler, "task_respawn_counter_mode_handler", 2048, NULL, 5, NULL);
    ESP_LOGI(TAG, "Application tasks started");
}