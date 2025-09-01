#include "freertos/FreeRTOS.h"

#ifndef GAME_MODE_H
#define GAME_MODE_H

typedef enum
{
    RESPAWN_COUNTER,
    BOMB,
    FLAG,
} GAME_MODE;

#endif // GAME_MODE_H