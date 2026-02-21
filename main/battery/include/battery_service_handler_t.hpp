#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "battery_state_type.hpp"

#ifndef BATTERY_SERVICE_HANDLER_H
#define BATTERY_SERVICE_HANDLER_H

class battery_service_handler_t
{
private:
    static const char *TAG;
    static i2c_master_dev_handle_t dev_handle;
    static float max_voltage;

    battery_service_handler_t();
    static int16_t read_registry(uint8_t reg);

public:
    static void task(void *pvParameter);
    static void init();
    static BATTERY_STATE_TYPE state();
    static float get_voltage();
    static float get_current();
};

#endif // BATTERY_SERVICE_HANDLER_H