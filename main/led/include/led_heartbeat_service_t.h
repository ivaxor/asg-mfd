#include "freertos/FreeRTOS.h"
#include "led_strip_rmt.h"

#ifndef LED_HEARTBEAT_SERVICE_H
#define LED_HEARTBEAT_SERVICE_H

class led_heartbeat_service_t
{
private:
    const char *TAG = "led_heartbeat_service_t";
    led_strip_handle_t led_strip;

public:
    void init();
    void task(void *pvParameter);
};

extern led_heartbeat_service_t led_heartbeat_service;

#endif // LED_HEARTBEAT_SERVICE_H