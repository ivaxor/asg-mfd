#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "include/battery_service_handler_t.hpp"
#include "include/battery_state_type.hpp"
#include "../buzzer/include/buzzer_beep_type.hpp"
#include "../buzzer/include/buzzer_event_queue_handler_t.hpp"

#define I2C_SDA_PIN GPIO_NUM_9
#define I2C_SCL_PIN GPIO_NUM_10
#define INA219_ADDR 0x40
#define INA219_REG_CONFIG 0x00
#define INA219_REG_SHUNT 0x01
#define INA219_REG_BUS 0x02

const char *battery_service_handler_t::TAG = "battery_service_handler_t";
i2c_master_dev_handle_t battery_service_handler_t::dev_handle;
float battery_service_handler_t::max_voltage = 0.0f;

void battery_service_handler_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    vTaskDelay(pdMS_TO_TICKS(5000));
    while (true)
    {
        BATTERY_STATE_TYPE battery_state = state();
        switch (battery_state)
        {
        case NORMAL:
        case UNKNOWN:
            break;

        case LOW:
            buzzer_event_queue_handler_t::add_to_queue(BUZZER_BEEP_TYPE::LOW_BATTERY);
            break;

        case CUTOFF:
            esp_restart();
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(300000));
    }
}

void battery_service_handler_t::init()
{
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = -1,
        .sda_io_num = (gpio_num_t)I2C_SDA_PIN,
        .scl_io_num = (gpio_num_t)I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = 1,
            .allow_pd = 0,
        },
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = INA219_ADDR,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

    uint16_t config = 0x399F;
    uint8_t config_data[3] = {INA219_REG_CONFIG, (uint8_t)(config >> 8), (uint8_t)(config & 0xFF)};
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, config_data, 3, -1));
}

BATTERY_STATE_TYPE battery_service_handler_t::state()
{
    float voltage = get_voltage();

    float cell_normal = 4.2f;
    float cell_low = 3.76f;
    float cell_cutoff = 3.5f;
    float cell_unknown = 3.0f;

    int cells = 1;
    do
    {
        if (voltage <= cell_unknown * cells)
            return UNKNOWN;

        if (voltage <= cell_cutoff * cells)
            return CUTOFF;

        if (voltage <= cell_low * cells)
            return LOW;

        if (voltage <= cell_normal * cells)
            return NORMAL;

        cells++;
    } while (true);

    ESP_LOGE(TAG, "Unsupported battery max voltage");
    return UNKNOWN;
}

float battery_service_handler_t::get_voltage()
{
    int16_t raw_bus = (int16_t)read_registry(INA219_REG_BUS);
    float voltage = (raw_bus >> 3) * 0.004f;

    if (voltage > max_voltage)
        max_voltage = voltage;

    ESP_LOGD(TAG, "Voltage: %.2f V", voltage);

    return voltage;
}

float battery_service_handler_t::get_current()
{
    int16_t raw_shunt = (int16_t)read_registry(INA219_REG_SHUNT);
    float current = raw_shunt * 0.0001f;

    ESP_LOGD(TAG, "Current: %.2f A", current);

    return current;
}

int16_t battery_service_handler_t::read_registry(uint8_t registry)
{
    uint8_t data[2];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &registry, 1, data, 2, -1));
    return (int16_t)((data[0] << 8) | data[1]);
}