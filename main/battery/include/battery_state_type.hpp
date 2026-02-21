#include "freertos/FreeRTOS.h"

#ifndef BATTERY_STATE_TYPE_H
#define BATTERY_STATE_TYPE_H

typedef enum
{
    NORMAL,
    LOW,
    CUTOFF,
    UNKNOWN,
} BATTERY_STATE_TYPE;

#endif