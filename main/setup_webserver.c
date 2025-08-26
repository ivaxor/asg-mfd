#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_http_server.h"

static const char *TAG = "setup_webserver";

static esp_err_t captive_portal_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "302 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "http://192.168.4.1/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}
static httpd_uri_t captive_portal_uri = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = captive_portal_handler,
    .user_ctx = NULL,
};

static esp_err_t root_handler(httpd_req_t *req)
{
    const char *html_content = "<!DOCTYPE html><html><body><h1>Привет, мир!</h1></body></html>";
    httpd_resp_send(req, html_content, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
static httpd_uri_t root_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_handler,
    .user_ctx = NULL,
};

static esp_err_t api_info_handler(httpd_req_t *req)
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
static httpd_uri_t api_info_uri = {
    .uri = "/api/info",
    .method = HTTP_GET,
    .handler = api_info_handler,
    .user_ctx = NULL,
};

void setup_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGI(TAG, "Error starting web server");
    }

    httpd_register_uri_handler(server, &root_uri);
    httpd_register_uri_handler(server, &api_info_uri);
    httpd_register_uri_handler(server, &captive_portal_uri);
}