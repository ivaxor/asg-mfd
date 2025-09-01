#include "freertos/FreeRTOS.h"
#include "game_mode_t.h"

#ifndef GAME_MODE_SERVICE_H
#define GAME_MODE_SERVICE_H

class game_mode_service_t
{
private:
    GAME_MODE mode;

public:
    GAME_MODE get();
    void replace(GAME_MODE new_mode);
};

extern game_mode_service_t game_mode_service;

#endif // GAME_MODE_SERVICE_H