#include "freertos/FreeRTOS.h"

#ifndef BUTTON_EVENT_H
#define BUTTON_EVENT_H

typedef enum
{
    CLICK,
    PRESS,
} BUTTON_EVENT_TYPE;

typedef struct
{
    uint8_t gpio_num;
    BUTTON_EVENT_TYPE type;
    int64_t duration;
} button_event_t;

#endif