#include "freertos/FreeRTOS.h"

#ifndef WEBSERVER_STATIC_HANDLERS_H
#define WEBSERVER_STATIC_HANDLERS_H

class webserver_static_handlers_t
{
private:    
    static esp_err_t index_html_handler(httpd_req_t *req);
    static esp_err_t main_js_handler(httpd_req_t *req);
    static esp_err_t styles_css_handler(httpd_req_t *req);
    static esp_err_t favicon_ico_handler(httpd_req_t *req);

    webserver_static_handlers_t();

public:
    static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
    static esp_err_t cors_handler(httpd_req_t *req);

    static void httpd_resp_set_hdr_cors(httpd_req_t *req);

    static const httpd_uri_t cors_uri;
    static const httpd_uri_t index_html_uri;
    static const httpd_uri_t main_js_uri;
    static const httpd_uri_t styles_css_uri;
    static const httpd_uri_t favicon_ico_uri;
};

#endif // WEBSERVER_STATIC_HANDLERS_H