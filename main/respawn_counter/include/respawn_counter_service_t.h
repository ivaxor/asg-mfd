#include "freertos/FreeRTOS.h"
#include "respawn_counter_info_t.h"
#include "../../button/include/button_event_t.h"

#ifndef RESPAWN_COUNTER_SERVICE_H
#define RESPAWN_COUNTER_SERVICE_H

class respawn_counter_service_t
{
private:
    static const char *TAG;
    static bool setup_mode;
    static uint8_t setup_mode_menu;
    static respawn_counter_info_t info;

    void short_press();
    void long_press();
    void setup_mode_short_press();
    void setup_mode_long_press();
    void draw_on_matrix_display();

public:
    void init();
    void task(void *pvParameter);
    void handle_button_event(button_event_t button_event);
    respawn_counter_info_t get();
    void replace(respawn_counter_info_t new_info);
};

extern respawn_counter_service_t respawn_counter_service;

#endif // RESPAWN_COUNTER_SERVICE_H