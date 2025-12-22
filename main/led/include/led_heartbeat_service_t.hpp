#include "freertos/FreeRTOS.h"
#include "led_strip_rmt.h"

#ifndef LED_HEARTBEAT_SERVICE_H
#define LED_HEARTBEAT_SERVICE_H

class led_heartbeat_service_t
{
private:
    static const char *TAG;
    static const uint8_t MAX_COLOR_VALUE;
    static const uint16_t CYCLE_LENGTH;

    static led_strip_handle_t led_strip;
    static led_strip_handle_t led;

    led_heartbeat_service_t();
    static uint8_t calculate_r(uint16_t step);
    static uint8_t calculate_g(uint16_t step);
    static uint8_t calculate_b(uint16_t step);

public:
    static void init();
    static void task(void *pvParameter);
};

#endif // LED_HEARTBEAT_SERVICE_H