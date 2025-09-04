#include "freertos/FreeRTOS.h"
#include "esp_http_server.h"

#ifndef WEBSERVER_SERVICE_H
#define WEBSERVER_SERVICE_H

class webserver_service_t
{
private:
    static const char *TAG;

    static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    void wifi_init_softap();
    void dhcp_set_captiveportal_url();
    httpd_handle_t start_webserver();

public:
    void init();
};

extern webserver_service_t webserver_service;

#endif