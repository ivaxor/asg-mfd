#include "freertos/FreeRTOS.h"

#ifndef BUTTON_RAW_EVENT_H
#define BUTTON_RAW_EVENT_H

typedef struct {
    uint8_t gpio_num;
    bool pressed;
    int64_t timestamp;
} button_raw_event_t;

#endif