#include "freertos/FreeRTOS.h"
#include "sdmmc_cmd.h"
#include "../../respawn_counter/include/respawn_counter_info_t.h"

#ifndef SD_CARD_SERVICE_H
#define SD_CARD_SERVICE_H

class sd_card_service_t
{
private:
    static const char *TAG;
    static const char *respawn_counter_info_file_path;
    
    static sdmmc_card_t *card;
    static bool card_initialized;

    sd_card_service_t();

public:
    static void init();
    static respawn_counter_info_t *read_respawn_counter_info();
    static void write_respawn_counter_info(respawn_counter_info_t *info);
};

#endif // SD_CARD_SERVICE_H