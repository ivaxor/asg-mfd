#include "freertos/FreeRTOS.h"
#include "game_mode_info_t.h"

#ifndef GAME_MODE_SERVICE_H
#define GAME_MODE_SERVICE_H

class game_mode_service_t
{
private:
    static const char *TAG;

    static game_mode_info_t info;
    static TaskHandle_t respawn_counter_task;

    static void replace_cleanup();
    static void replace_init_new(GAME_MODE new_mode);
    
    game_mode_service_t();

public:
    static void init();
    static void task(void *pvParameter);
    static game_mode_info_t *get();
    static void replace(GAME_MODE new_mode);
};

#endif // GAME_MODE_SERVICE_H