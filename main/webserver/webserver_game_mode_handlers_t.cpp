#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "cJSON.h"
#include "include/webserver_game_mode_handlers_t.hpp"
#include "include/webserver_static_handlers_t.hpp"
#include "../game_mode/include/game_mode_service_t.hpp"
#include "../battery/include/battery_service_handler_t.hpp"

const httpd_uri_t webserver_game_mode_handlers_t::game_mode_info_options_uri = {
    .uri = "/api/game-mode",
    .method = HTTP_OPTIONS,
    .handler = webserver_static_handlers_t::cors_handler,
    .user_ctx = NULL,
};

esp_err_t webserver_game_mode_handlers_t::game_mode_info_get_handler(httpd_req_t *req)
{
    float voltage = battery_service_handler_t::get_voltage();
    float current = battery_service_handler_t::get_current();
    game_mode_info_t *info = game_mode_service_t::get();

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "voltage", voltage);
    cJSON_AddNumberToObject(root, "current", current);
    cJSON_AddNumberToObject(root, "mode", info->mode);
    cJSON_AddNumberToObject(root, "start_timestamp", info->start_timestamp);
    cJSON_AddNumberToObject(root, "timestamp", esp_timer_get_time());

    const char *json_string = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    webserver_static_handlers_t::httpd_resp_set_hdr_cors(req);
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);

    cJSON_Delete(root);
    free((void *)json_string);
    return ESP_OK;
}
const httpd_uri_t webserver_game_mode_handlers_t::game_mode_info_get_uri = {
    .uri = "/api/game-mode",
    .method = HTTP_GET,
    .handler = game_mode_info_get_handler,
    .user_ctx = NULL,
};

esp_err_t webserver_game_mode_handlers_t::game_mode_info_post_handler(httpd_req_t *req)
{
    char content[req->content_len + 1];
    int ret = httpd_req_recv(req, content, req->content_len);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            httpd_resp_send_408(req);

        return ESP_FAIL;
    }

    content[req->content_len] = '\0';
    cJSON *root = cJSON_Parse(content);
    if (!root)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON received.");
        return ESP_FAIL;
    }

    cJSON *mode = cJSON_GetObjectItemCaseSensitive(root, "mode");

    game_mode_service_t::replace((GAME_MODE)mode->valueint);

    cJSON_Delete(root);
    webserver_static_handlers_t::httpd_resp_set_hdr_cors(req);
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}
const httpd_uri_t webserver_game_mode_handlers_t::game_mode_info_post_uri = {
    .uri = "/api/game-mode",
    .method = HTTP_POST,
    .handler = game_mode_info_post_handler,
    .user_ctx = NULL,
};