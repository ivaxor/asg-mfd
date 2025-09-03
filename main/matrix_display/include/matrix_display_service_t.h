#include "freertos/FreeRTOS.h"
#include "max7219.h"

#ifndef MATRIX_DISPLAY_SERVICE_H
#define MATRIX_DISPLAY_SERVICE_H

class matrix_display_service_t
{
private:
    const char *TAG = "matrix_display_service_t";
    max7219_t device;

public:
    void init();
};

extern matrix_display_service_t matrix_display_service;

#endif // MATRIX_DISPLAY_SERVICE_H