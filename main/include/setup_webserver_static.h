#ifndef SETUP_WEBSERVER_STATIC_H
#define SETUP_WEBSERVER_STATIC_H

#include "esp_http_server.h"

extern const httpd_uri_t index_html_uri;
extern const httpd_uri_t main_js_uri;
extern const httpd_uri_t styles_css_uri;
extern const httpd_uri_t favicon_ico_uri;
extern const httpd_uri_t api_info_uri;
extern const httpd_uri_t generate_204_uri;

#endif // SETUP_WEBSERVER_STATIC_H