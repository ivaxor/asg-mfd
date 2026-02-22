#include "driver/sdspi_host.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "max7219.h"
#include "include/matrix_display_service_t.hpp"

#define MATRIX_DISPLAY_CS_PIN GPIO_NUM_0
#define MATRIX_DISPLAY_SEGMENTS 8

const char *matrix_display_service_t::TAG = "matrix_display_service_t";

const uint8_t matrix_display_service_t::blank[MATRIX_DISPLAY_SEGMENTS] = {};

// https://xantorohara.github.io/led-matrix-editor/
const uint64_t matrix_display_service_t::digits_upper_parts[] = {
    0xc6c6c6fefe000000,
    0x3030303e3e000000,
    0xc0c0c0fefe000000,
    0xc0c0c0fefe000000,
    0xc6c6c6c6c6000000,
    0x060606fefe000000,
    0x060606fefe000000,
    0xc0c0c0fefe000000,
    0xc6c6c6fefe000000,
    0xc6c6c6fefe000000,
};
const uint64_t matrix_display_service_t::digits_lower_parts[] = {
    0x00fefec6c6c6c6c6,
    0x00fefe3030303030,
    0x00fefe060606fefe,
    0x00fefec0c0c0fefe,
    0x00c0c0c0c0c0fefe,
    0x00fefec0c0c0fefe,
    0x00fefec6c6c6fefe,
    0x00c0c0c0c0c0c0c0,
    0x00fefec6c6c6fefe,
    0x00fefec0c0c0fefe,
};
const uint64_t matrix_display_service_t::special_characters[] = {
    0x1818187e7e666600, // WRENCH
    0x0002044850607800, // ARROW_UP_RIGHT
    0x0078605048040200, // ARROW_DOWN_RIGHT
    0x0000081422400000, // CHECK_MARK
    0x0048241212244800, // REWIND
    0x003c2424243c1800, // BATTERY
};
max7219_t matrix_display_service_t::device;
SemaphoreHandle_t matrix_display_service_t::mutex;

void matrix_display_service_t::init()
{
    ESP_LOGD(TAG, "Initializing");

    mutex = xSemaphoreCreateMutex();
    if (mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create matrix display mutex");
        return;
    }

    device = {
        .digits = 0,
        .cascade_size = MATRIX_DISPLAY_SEGMENTS,
        .mirrored = true,
    };

    // Initialize the device descriptor and display chain
    ESP_ERROR_CHECK(max7219_init_desc(&device, SPI2_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, MATRIX_DISPLAY_CS_PIN));
    ESP_ERROR_CHECK(max7219_init(&device));
    ESP_ERROR_CHECK(max7219_set_brightness(&device, MAX7219_MAX_BRIGHTNESS / 2));
}

void matrix_display_service_t::uninit()
{
    clear_all();

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE)
    {
        max7219_set_shutdown_mode(&device, true);
        max7219_free_desc(&device);
        spi_bus_free(SPI2_HOST);
    }

    xSemaphoreGive(mutex);
}

void matrix_display_service_t::clear(uint8_t display)
{
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE)
        max7219_draw_image_8x8(&matrix_display_service_t::device, display * MATRIX_DISPLAY_SEGMENTS, (uint8_t *)blank);

    xSemaphoreGive(mutex);
}

void matrix_display_service_t::clear_all()
{
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE)
        max7219_clear(&device);

    xSemaphoreGive(mutex);
}

void matrix_display_service_t::draw_special_character(uint8_t display, MATRIX_SPECIAL_CHARACTER character)
{
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE)
        max7219_draw_image_8x8(&matrix_display_service_t::device, display * MATRIX_DISPLAY_SEGMENTS, (uint8_t *)special_characters + character * MATRIX_DISPLAY_SEGMENTS);

    xSemaphoreGive(mutex);
}

void matrix_display_service_t::draw_tall_number(uint8_t display1, uint8_t display2, uint8_t number)
{
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE)
    {
        max7219_draw_image_8x8(&matrix_display_service_t::device, display1 * MATRIX_DISPLAY_SEGMENTS, (uint8_t *)digits_upper_parts + number * MATRIX_DISPLAY_SEGMENTS);
        max7219_draw_image_8x8(&matrix_display_service_t::device, display2 * MATRIX_DISPLAY_SEGMENTS, (uint8_t *)digits_lower_parts + number * MATRIX_DISPLAY_SEGMENTS);
    }

    xSemaphoreGive(mutex);
}