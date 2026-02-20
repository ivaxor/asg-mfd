#include "freertos/FreeRTOS.h"

#ifndef BUZZER_BEEP_TYPE_H
#define BUZZER_BEEP_TYPE_H

typedef enum
{
    RESPAWN_NO_TOKENS,
    RESPAWN_TOKEN_DECREMENT,
    RESPAWN_BATCH,
    SETUP_MODE,
    LOW_BATTERY,
} BUZZER_BEEP_TYPE;

#endif