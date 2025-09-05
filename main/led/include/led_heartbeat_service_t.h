#include "freertos/FreeRTOS.h"
#include "led_strip_rmt.h"

#ifndef LED_HEARTBEAT_SERVICE_H
#define LED_HEARTBEAT_SERVICE_H

class led_heartbeat_service_t
{
private:
    static const char *TAG;

    static led_strip_handle_t led_strip;
    
    led_heartbeat_service_t();

public:
    static void init();
    static void task(void *pvParameter);
};

#endif // LED_HEARTBEAT_SERVICE_H