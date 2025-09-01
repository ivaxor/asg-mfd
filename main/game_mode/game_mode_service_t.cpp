#include "freertos/FreeRTOS.h"
#include "include/game_mode_service_t.h"

game_mode_service_t game_mode_service;

GAME_MODE game_mode_service_t::get()
{
    return mode;
}

void game_mode_service_t::replace(GAME_MODE new_mode)
{
    mode = new_mode;

    // TODO: Send a message to game mode handlers
}