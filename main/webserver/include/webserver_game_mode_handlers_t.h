#include "freertos/FreeRTOS.h"

#ifndef WEBSERVER_GAME_MODE_HANDLERS_H
#define WEBSERVER_GAME_MODE_HANDLERS_H

class webserver_game_mode_handlers_t
{
private:
    static esp_err_t game_mode_info_get_handler(httpd_req_t *req);
    static esp_err_t game_mode_info_post_handler(httpd_req_t *req);
    
    webserver_game_mode_handlers_t();

public:
    static const httpd_uri_t game_mode_info_options_uri;
    static const httpd_uri_t game_mode_info_get_uri;
    static const httpd_uri_t game_mode_info_post_uri;
};

#endif // WEBSERVER_GAME_MODE_HANDLERS_H