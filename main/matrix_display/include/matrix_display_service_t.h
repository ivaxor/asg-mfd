#include "freertos/FreeRTOS.h"
#include "max7219.h"

#ifndef MATRIX_DISPLAY_SERVICE_H
#define MATRIX_DISPLAY_SERVICE_H

typedef enum
{
    WRENCH,
    GROUP,
    ARROW_UP_RIGHT,
    ARROW_DOWN_RIGHT,
} MATRIX_SPECIAL_CHARACTER;

class matrix_display_service_t
{
private:
    static const char *TAG;
    static const uint8_t blank[];
    static const uint64_t digits_upper_parts[];
    static const uint64_t digits_lower_parts[];
    static const uint64_t special_characters[];
    static max7219_t device;

public:
    void init();
    void clear(uint8_t display);
    void clear_all();
    void draw_special_character(uint8_t display, MATRIX_SPECIAL_CHARACTER character);
    void draw_tall_number(uint8_t display1, uint8_t display2, uint8_t number);
};

extern matrix_display_service_t matrix_display_service;

#endif // MATRIX_DISPLAY_SERVICE_H