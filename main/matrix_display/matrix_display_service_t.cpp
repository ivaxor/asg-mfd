#include "driver/sdspi_host.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "max7219.h"
#include "include/matrix_display_service_t.h"

const char *matrix_display_service_t::TAG = "matrix_display_service_t";

const uint8_t matrix_display_service_t::blank[8] = {};
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
};
max7219_t matrix_display_service_t::device;
SemaphoreHandle_t matrix_display_service_t::mutex;

void matrix_display_service_t::init()
{
    mutex = xSemaphoreCreateMutex();
    if (mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create matrix display mutex");
        return;
    }

    device = {
        .digits = 0,
        .cascade_size = 8,
        .mirrored = true,
    };

    // Initialize the device descriptor and display chain
    ESP_ERROR_CHECK(max7219_init_desc(&device, SDSPI_DEFAULT_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, GPIO_NUM_10));
    ESP_ERROR_CHECK(max7219_init(&device));
    ESP_ERROR_CHECK(max7219_set_brightness(&device, MAX7219_MAX_BRIGHTNESS));
}

void matrix_display_service_t::uninit()
{
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
        max7219_draw_image_8x8(&matrix_display_service_t::device, display * 8, (uint8_t *)blank);

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
        max7219_draw_image_8x8(&matrix_display_service_t::device, display * 8, (uint8_t *)special_characters + character * 8);

    xSemaphoreGive(mutex);
}

void matrix_display_service_t::draw_tall_number(uint8_t display1, uint8_t display2, uint8_t number)
{
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE)
    {
        max7219_draw_image_8x8(&matrix_display_service_t::device, display1 * 8, (uint8_t *)digits_upper_parts + number * 8);
        max7219_draw_image_8x8(&matrix_display_service_t::device, display2 * 8, (uint8_t *)digits_lower_parts + number * 8);
    }

    xSemaphoreGive(mutex);
}