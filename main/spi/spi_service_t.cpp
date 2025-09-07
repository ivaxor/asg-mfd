#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "include/spi_service_t.h"

void spi_service_t::init()
{
    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = GPIO_NUM_11,
        .miso_io_num = GPIO_NUM_13,
        .sclk_io_num = GPIO_NUM_12,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi_bus_config, SDSPI_DEFAULT_DMA));
}