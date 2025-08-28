#include "esp_http_server.h"
#include "cJSON.h"

// Handler for the root page (index.html)
extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[] asm("_binary_index_html_end");
esp_err_t index_html_handler(httpd_req_t *req)
{
    const uint32_t index_html_len = index_html_end - index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html_start, index_html_len);
    return ESP_OK;
}
const httpd_uri_t index_html_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_html_handler,
};

// Handler for main.js
extern const char main_js_start[] asm("_binary_main_js_start");
extern const char main_js_end[] asm("_binary_main_js_end");
esp_err_t main_js_handler(httpd_req_t *req)
{
    const uint32_t main_js_len = main_js_end - main_js_start;
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, main_js_start, main_js_len);
    return ESP_OK;
}
const httpd_uri_t main_js_uri = {
    .uri = "/main.js",
    .method = HTTP_GET,
    .handler = main_js_handler,
};

// Handler for styles.css
extern const char styles_css_start[] asm("_binary_styles_css_start");
extern const char styles_css_end[] asm("_binary_styles_css_end");
esp_err_t styles_css_handler(httpd_req_t *req)
{
    const uint32_t styles_css_len = styles_css_end - styles_css_start;
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, styles_css_start, styles_css_len);
    return ESP_OK;
}
const httpd_uri_t styles_css_uri = {
    .uri = "/styles.css",
    .method = HTTP_GET,
    .handler = styles_css_handler,
};

// Handler for favicon.ico
extern const char favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const char favicon_ico_end[] asm("_binary_favicon_ico_end");
esp_err_t favicon_ico_handler(httpd_req_t *req)
{
    const uint32_t favicon_ico_len = favicon_ico_end - favicon_ico_start;
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, favicon_ico_start, favicon_ico_len);
    return ESP_OK;
}
const httpd_uri_t favicon_ico_uri = {
    .uri = "/favicon.ico",
    .method = HTTP_GET,
    .handler = favicon_ico_handler,
};

// Handler for the API info endpoint
esp_err_t api_info_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "uptime", xTaskGetTickCount() * portTICK_PERIOD_MS / 1000);
    cJSON_AddStringToObject(root, "status", "ok");
    const char *json_string = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);
    cJSON_Delete(root);
    free((void *)json_string);
    return ESP_OK;
}
const httpd_uri_t api_info_uri = {
    .uri = "/api/info",
    .method = HTTP_GET,
    .handler = api_info_handler,
    .user_ctx = NULL,
};