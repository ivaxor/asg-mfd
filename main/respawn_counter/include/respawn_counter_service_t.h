#include "freertos/FreeRTOS.h"
#include "respawn_counter_info_t.h"
#include "../../button/include/button_event_t.h"

#ifndef RESPAWN_COUNTER_SERVICE_H
#define RESPAWN_COUNTER_SERVICE_H

class respawn_counter_service_t
{
private:
    const char *TAG = "respawn_counter_service_t";
    bool setup_mode;
    respawn_counter_info_t info;

    void short_press();
    void long_press();
    void setup_mode_short_press();
    void setup_mode_long_press();

public:
    void init();
    void task(void *pvParameter);
    void handle_button_event(button_event_t button_event);
    respawn_counter_info_t get();
    void replace(respawn_counter_info_t new_info);
};

extern respawn_counter_service_t respawn_counter_service;

#endif // RESPAWN_COUNTER_SERVICE_H