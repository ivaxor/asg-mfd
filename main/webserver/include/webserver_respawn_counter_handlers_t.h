#include "freertos/FreeRTOS.h"

#ifndef WEBSERVER_RESPAWN_COUNTER_HANDLERS_H
#define WEBSERVER_RESPAWN_COUNTER_HANDLERS_H

class webserver_respawn_counter_handlers_t
{
private:
    static esp_err_t respawn_counter_info_get_handler(httpd_req_t *req);
    static esp_err_t respawn_counter_info_post_handler(httpd_req_t *req);
    
    webserver_respawn_counter_handlers_t();

public:
    static const httpd_uri_t respawn_counter_info_options_uri;
    static const httpd_uri_t respawn_counter_info_get_uri;
    static const httpd_uri_t respawn_counter_info_post_uri;
};

#endif // WEBSERVER_RESPAWN_COUNTER_HANDLERS_H