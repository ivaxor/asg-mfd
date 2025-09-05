#include "freertos/FreeRTOS.h"

#ifndef GAME_MODE_INFO_H
#define GAME_MODE_INFO_H

typedef enum
{
    RESPAWN_COUNTER,
    BOMB,
    FLAG,
} GAME_MODE;

typedef struct
{
    GAME_MODE mode;
    int64_t start_timestamp;
} game_mode_info_t;

#endif // GAME_MODE_INFO_H