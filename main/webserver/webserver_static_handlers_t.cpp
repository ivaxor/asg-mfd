#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "include/webserver_static_handlers_t.hpp"

esp_err_t webserver_static_handlers_t::http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    httpd_resp_set_status(req, "302 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

esp_err_t webserver_static_handlers_t::cors_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr_cors(req);
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

void webserver_static_handlers_t::httpd_resp_set_hdr_cors(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
}

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[] asm("_binary_index_html_end");
esp_err_t webserver_static_handlers_t::index_html_handler(httpd_req_t *req)
{
    const uint32_t index_html_len = index_html_end - index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html_start, index_html_len);
    return ESP_OK;
}
const httpd_uri_t webserver_static_handlers_t::index_html_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_html_handler,
};

extern const char main_js_start[] asm("_binary_main_js_start");
extern const char main_js_end[] asm("_binary_main_js_end");
esp_err_t webserver_static_handlers_t::main_js_handler(httpd_req_t *req)
{
    const uint32_t main_js_len = main_js_end - main_js_start;
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, main_js_start, main_js_len);
    return ESP_OK;
}
const httpd_uri_t webserver_static_handlers_t::main_js_uri = {
    .uri = "/main.js",
    .method = HTTP_GET,
    .handler = main_js_handler,
};

extern const char scripts_js_start[] asm("_binary_scripts_js_start");
extern const char scripts_js_end[] asm("_binary_scripts_js_end");
esp_err_t webserver_static_handlers_t::scripts_js_handler(httpd_req_t *req)
{
    const uint32_t scripts_js_len = scripts_js_end - scripts_js_start;
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, scripts_js_start, scripts_js_len);
    return ESP_OK;
}
const httpd_uri_t webserver_static_handlers_t::scripts_js_uri = {
    .uri = "/scripts.js",
    .method = HTTP_GET,
    .handler = scripts_js_handler,
};

extern const char styles_css_start[] asm("_binary_styles_css_start");
extern const char styles_css_end[] asm("_binary_styles_css_end");
esp_err_t webserver_static_handlers_t::styles_css_handler(httpd_req_t *req)
{
    const uint32_t styles_css_len = styles_css_end - styles_css_start;
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, styles_css_start, styles_css_len);
    return ESP_OK;
}
const httpd_uri_t webserver_static_handlers_t::styles_css_uri = {
    .uri = "/styles.css",
    .method = HTTP_GET,
    .handler = styles_css_handler,
};

extern const char favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const char favicon_ico_end[] asm("_binary_favicon_ico_end");
esp_err_t webserver_static_handlers_t::favicon_ico_handler(httpd_req_t *req)
{
    const uint32_t favicon_ico_len = favicon_ico_end - favicon_ico_start;
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, favicon_ico_start, favicon_ico_len);
    return ESP_OK;
}
const httpd_uri_t webserver_static_handlers_t::favicon_ico_uri = {
    .uri = "/favicon.ico",
    .method = HTTP_GET,
    .handler = favicon_ico_handler,
};