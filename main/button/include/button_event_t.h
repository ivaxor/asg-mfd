#include "freertos/FreeRTOS.h"

#ifndef BUTTON_EVENT_H
#define BUTTON_EVENT_H

typedef enum
{
    DEPRESSED,
    PRESSED,    
} BUTTON_EVENT_STATE;

typedef struct
{
    uint8_t gpio_num;
    BUTTON_EVENT_STATE state;
    int64_t duration;
} button_event_t;

#endif