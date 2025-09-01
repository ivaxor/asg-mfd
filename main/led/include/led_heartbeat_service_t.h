
#include "freertos/FreeRTOS.h"

#ifndef LED_HEARTBEAT_SERVICE_H
#define LED_HEARTBEAT_SERVICE_H

#define LED_HEARTBEAT_PIN GPIO_NUM_5

class led_heartbeat_service_t
{
private:
    const char *TAG = "led_heartbeat_service_t";

public:
    void init();
    void task(void *pvParameter);
};

extern led_heartbeat_service_t led_heartbeat_service;

#endif // LED_HEARTBEAT_SERVICE_H