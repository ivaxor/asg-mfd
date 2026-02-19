#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "driver/i2c_master.h"
#include "buzzer/include/buzzer_event_queue_handler_t.hpp"
#include "game_mode/include/game_mode_service_t.hpp"
#include "led_heartbeat/include/led_heartbeat_service_t.hpp"
#include "sd_card/include/sd_card_service_t.hpp"
#include "spi/include/spi_service_t.hpp"
#include "webserver/include/webserver_service_t.hpp"

#define I2C_SDA_PIN GPIO_NUM_9
#define I2C_SCL_PIN GPIO_NUM_10
#define INA219_ADDR 0x40

// INA219
#define REG_CONFIG 0x00
#define REG_SHUNT 0x01
#define REG_BUS 0x02
#define REG_CALIB 0x05

static const char *TAG = "app_main";
i2c_master_dev_handle_t dev_handle;

int16_t read_reg(uint8_t reg)
{
    uint8_t data[2];
    i2c_master_transmit_receive(dev_handle, &reg, 1, data, 2, -1);
    return (int16_t)((data[0] << 8) | data[1]);
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting application setup");
    spi_service_t::init();
    sd_card_service_t::init();
    // TODO: Remove after issue resovled:
    /*
I (809) led_strip_service_t: Initializing
E (819) rmt: rmt_tx_register_to_group(152): no free tx channels
E (819) rmt: rmt_new_tx_channel(286): register channel failed
E (829) led_strip_rmt: led_strip_new_rmt_device(154): create RMT TX channel failed
    */
    //led_heartbeat_service_t::init();
    webserver_service_t::init();
    buzzer_event_queue_handler_t::init();
    game_mode_service_t::init();
    ESP_LOGI(TAG, "Application setup completed");

    ESP_LOGI(TAG, "Starting application tasks");
    xTaskCreate(buzzer_event_queue_handler_t::task, "buzzer_event_queue_handler_t", 4096, NULL, 5, NULL);
    xTaskCreate(game_mode_service_t::task, "game_mode_service_t", 4096, NULL, 5, NULL);
    // TODO: Remove after issue resolved
    // xTaskCreate(led_heartbeat_service_t::task, "led_heartbeat_service_t", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Application tasks started");

    buzzer_event_queue_handler_t::add_to_queue(BUZZER_BEEP_TYPE::SETUP_MODE);

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

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = INA219_ADDR,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    while (1)
    {
        int16_t raw_bus = (int16_t)read_reg(REG_BUS);
        float voltage_v = (raw_bus >> 3) * 0.004f;

        int16_t raw_shunt = (int16_t)read_reg(REG_SHUNT);
        float shunt_v = raw_shunt * 0.00001f;
        float current_ma = (shunt_v / 0.1f) * 1000.0f;

        printf("Voltage: %.2f V | Current: %.2f mA\n", voltage_v, current_ma);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}