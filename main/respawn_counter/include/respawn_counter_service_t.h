#include "freertos/FreeRTOS.h"
#include "respawn_counter_info_t.h"
#include "../../button/include/button_event_t.h"

#ifndef RESPAWN_COUNTER_SERVICE_H
#define RESPAWN_COUNTER_SERVICE_H

class respawn_counter_service_t
{
private:
    static const char *TAG;

    static respawn_counter_info_t info;
    static bool blink;
    static bool setup_mode;
    static RESPAWN_COUNTER_SETUP_MENU setup_mode_menu;    
    static respawn_counter_info_t setup_mode_info;

    static void short_press();
    static void long_press();
    static void setup_mode_short_press();
    static void setup_mode_long_press();
    static void render_setup_on_matrix_display();
    static void render_info_on_matrix_display();

public:
    static void init();
    static void uninit();
    static void task(void *pvParameter);
    static void handle_button_event(button_event_t button_event);
    static respawn_counter_info_t *get();
    static void replace(respawn_counter_info_t new_info);
};

#endif // RESPAWN_COUNTER_SERVICE_H