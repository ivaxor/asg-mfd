#include "freertos/FreeRTOS.h"

#ifndef SPI_SERVICE_H
#define SPI_SERVICE_H

class spi_service_t
{
private:
    spi_service_t();

public:
    static void init();
};

#endif // SPI_SERVICE_H