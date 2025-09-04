#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "button/include/button_event_queue_handler_t.h"
#include "button/include/button_isr_handler_t.h"
#include "button/include/button_raw_event_queue_handler_t.h"
#include "buzzer/include/buzzer_event_queue_handler_t.h"
#include "led/include/led_heartbeat_service_t.h"
#include "matrix_display/include/matrix_display_service_t.h"
#include "respawn_counter/include/respawn_counter_service_t.h"
#include "webserver/include/webserver_service_t.h"

static const char *TAG = "app_main";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting application setup");
    button_event_queue_handler.init();
    button_isr_handler.init();
    button_raw_event_queue_handler.init();
    buzzer_event_queue_handler.init();
    led_heartbeat_service_t::init();
    matrix_display_service.init();
    webserver_service.init();
    respawn_counter_service.init();
    ESP_LOGI(TAG, "Application setup completed");

    ESP_LOGI(TAG, "Starting application tasks");
    xTaskCreate([](void *param)
                { static_cast<button_event_queue_handler_t *>(param)->task(nullptr); },
                "button_event_queue_handler_task",
                4096,
                &button_event_queue_handler,
                5,
                NULL);
    xTaskCreate([](void *param)
                { static_cast<button_raw_event_queue_handler_t *>(param)->task(nullptr); },
                "button_raw_event_queue_handler_task",
                4096,
                &button_raw_event_queue_handler,
                5,
                NULL);
    xTaskCreate([](void *param)
                { static_cast<buzzer_event_queue_handler_t *>(param)->task(nullptr); },
                "buzzer_event_queue_handler_task",
                4096,
                &buzzer_event_queue_handler,
                5,
                NULL);
    xTaskCreate(led_heartbeat_service_t::task, "led_heartbeat_service_t", 4096, NULL, 5, NULL);
    xTaskCreate([](void *param)
                { static_cast<respawn_counter_service_t *>(param)->task(nullptr); },
                "respawn_counter_service_task",
                4096,
                &respawn_counter_service,
                5,
                NULL);
    ESP_LOGI(TAG, "Application tasks started");
}