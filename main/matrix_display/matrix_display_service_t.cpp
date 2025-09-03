#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "max7219.h"
#include "include/matrix_display_service_t.h"

#define NUM_DISPLAYS 8           // 4x2 matrix display array = 8 displays
#define PIN_NUM_CS GPIO_NUM_10   // Chip Select pin
#define PIN_NUM_MOSI GPIO_NUM_11 // MOSI pin
#define PIN_NUM_CLK GPIO_NUM_12  // CLK pin

matrix_display_service_t matrix_display_service;

void matrix_display_service_t::init()
{
    // Configure SPI bus
    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1, // Not used for MAX7219
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
        .flags = 0,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_CH_AUTO));

    // Configure the MAX7219 device
    device = {
        .digits = 0,
        .cascade_size = NUM_DISPLAYS,
        .mirrored = true,
    };

    // Initialize the device descriptor and display chain
    ESP_ERROR_CHECK(max7219_init_desc(&device, SPI2_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, PIN_NUM_CS));
    ESP_ERROR_CHECK(max7219_init(&device));

    // Clear all displays
    max7219_clear(&device);

    const uint8_t heart[8] = {
        0b00000000,
        0b01100110,
        0b11111111,
        0b11111111,
        0b11111111,
        0b01111110,
        0b00111100,
        0b00011000,
    };

    // Draw the heart pattern on the first display
    max7219_draw_image_8x8(&device, 0, (uint8_t *)heart);
    max7219_draw_image_8x8(&device, 2*8, (uint8_t *)heart);
    max7219_draw_image_8x8(&device, 5*8, (uint8_t *)heart);
    max7219_draw_image_8x8(&device, 7*8, (uint8_t *)heart);
}