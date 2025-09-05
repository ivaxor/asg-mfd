#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "include/webserver_respawn_counter_handlers_t.h"
#include "../respawn_counter/include/respawn_counter_service_t.h"

esp_err_t webserver_respawn_counter_handlers_t::respawn_counter_info_get_handler(httpd_req_t *req)
{
    respawn_counter_info_t *info = respawn_counter_service_t::get();

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "current_respawn_tokens", info->current_respawn_tokens);
    cJSON_AddNumberToObject(root, "respawn_tokens", info->respawn_tokens);

    cJSON *policies = cJSON_AddArrayToObject(root, "policies");
    for (uint8_t i = 0; i < info->policies_length; i++)
    {
        cJSON *policy = cJSON_CreateObject();
        cJSON_AddBoolToObject(policy, "enabled", info->policies[i].enabled);
        cJSON_AddNumberToObject(policy, "priority", info->policies[i].priority);
        cJSON_AddNumberToObject(policy, "min", info->policies[i].min);
        cJSON_AddNumberToObject(policy, "max", info->policies[i].max);
        cJSON_AddItemToArray(policies, policy);
    }
    cJSON_AddNumberToObject(root, "policies_length", info->policies_length);

    const char *json_string = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
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

    cJSON *respawn_tokens = cJSON_GetObjectItemCaseSensitive(root, "respawn_tokens");
    cJSON *current_respawn_tokens = cJSON_GetObjectItemCaseSensitive(root, "current_respawn_tokens");
    cJSON *policies = cJSON_GetObjectItemCaseSensitive(root, "policies");
    if (!cJSON_IsNumber(respawn_tokens) || !cJSON_IsNumber(current_respawn_tokens) || !cJSON_IsArray(policies))
    {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid data types for one or more fields.");
        return ESP_FAIL;
    }

    respawn_counter_info_t info;
    info.respawn_tokens = respawn_tokens->valueint;
    info.current_respawn_tokens = current_respawn_tokens->valueint;

    uint8_t policies_length = cJSON_GetArraySize(policies);
    info.policies = new respawn_counter_policy_t[policies_length];
    info.policies_length = policies_length;

    for (uint8_t i = 0; i < policies_length; i++)
    {
        cJSON *policy = cJSON_GetArrayItem(policies, i);
        if (cJSON_IsObject(policy))
        {
            cJSON *enabled = cJSON_GetObjectItemCaseSensitive(policy, "enabled");
            cJSON *priority = cJSON_GetObjectItemCaseSensitive(policy, "priority");
            cJSON *min = cJSON_GetObjectItemCaseSensitive(policy, "min");
            cJSON *max = cJSON_GetObjectItemCaseSensitive(policy, "max");
            cJSON *batch_size = cJSON_GetObjectItemCaseSensitive(policy, "batch_size");

            if (!cJSON_IsBool(enabled) || !cJSON_IsNumber(priority) || !cJSON_IsNumber(min) || !cJSON_IsNumber(max) || !cJSON_IsNumber(batch_size))
            {
                cJSON_Delete(root);
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid data types for one or more fields.");
                return ESP_FAIL;
            }

            info.policies[i].enabled = cJSON_IsTrue(enabled);
            info.policies[i].priority = priority->valueint;
            info.policies[i].min = min->valueint;
            info.policies[i].max = max->valueint;
            info.policies[i].batch_size = batch_size->valueint;
        }
    }

    respawn_counter_service_t::replace(info);

    cJSON_Delete(root);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}
const httpd_uri_t webserver_respawn_counter_handlers_t::respawn_counter_info_post_uri = {
    .uri = "/api/respawn-counter",
    .method = HTTP_POST,
    .handler = respawn_counter_info_post_handler,
    .user_ctx = NULL,
};