#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "include/webserver_respawn_counter_handlers_t.h"

webserver_respawn_counter_handlers_t webserver_respawn_counter_handlers;

esp_err_t webserver_respawn_counter_handlers_t::respawn_counter_info_get_handler(httpd_req_t *req)
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
const httpd_uri_t webserver_respawn_counter_handlers_t::respawn_counter_info_get_uri = {
    .uri = "/api/respawn-counter",
    .method = HTTP_GET,
    .handler = respawn_counter_info_get_handler,
    .user_ctx = NULL,
};

esp_err_t webserver_respawn_counter_handlers_t::respawn_counter_info_post_handler(httpd_req_t *req)
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
const httpd_uri_t webserver_respawn_counter_handlers_t::respawn_counter_info_post_uri = {
    .uri = "/api/respawn-counter",
    .method = HTTP_POST,
    .handler = respawn_counter_info_post_handler,
    .user_ctx = NULL,
};