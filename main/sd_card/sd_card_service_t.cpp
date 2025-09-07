#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "include/sd_card_service_t.h"

#define MOUNT_POINT "/sdcard"

const char *sd_card_service_t::TAG = "sd_card_service_t";
const char *sd_card_service_t::respawn_counter_info_file_path = MOUNT_POINT "/respawn_counter_info.json";
sdmmc_card_t *sd_card_service_t::card;

void sd_card_service_t::init()
{
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_DEFAULT / 2;

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = GPIO_NUM_46;
    slot_config.host_id = static_cast<spi_host_device_t>(host.slot);

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    ESP_LOGI(TAG, "Mounting SD card file system");

    ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card));
    sdmmc_card_print_info(stdout, card);

    ESP_LOGI(TAG, "SD card file system mounted");
}

respawn_counter_info_t *sd_card_service_t::read_respawn_counter_info()
{
    FILE *file = fopen(respawn_counter_info_file_path, "r");
    if (!file)
    {
        ESP_LOGW(TAG, "Failed to open respawn counter info file for reading");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = new char[file_size + 1];
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(content);
    if (!json)
    {
        ESP_LOGW(TAG, "Failed to read respawn counter info file");
        delete[] content;
        return NULL;
    }

    delete[] content;

    cJSON *respawn_tokens = cJSON_GetObjectItemCaseSensitive(json, "respawn_tokens");
    cJSON *current_respawn_tokens = cJSON_GetObjectItemCaseSensitive(json, "current_respawn_tokens");
    cJSON *policies = cJSON_GetObjectItemCaseSensitive(json, "policies");
    cJSON *policies_length = cJSON_GetObjectItemCaseSensitive(json, "policies_length");
    if (!cJSON_IsNumber(respawn_tokens) || !cJSON_IsNumber(current_respawn_tokens) || !cJSON_IsArray(policies) || !cJSON_IsNumber(policies_length))
    {
        ESP_LOGE(TAG, "Failed to read respawn counter info file properties");
        cJSON_Delete(json);
        return NULL;
    }

    respawn_counter_info_t *info = new respawn_counter_info_t();
    info->respawn_tokens = respawn_tokens->valueint;
    info->current_respawn_tokens = current_respawn_tokens->valueint;

    info->policies_length = policies_length->valueint;
    info->policies = new respawn_counter_policy_t[info->policies_length];

    for (uint8_t i = 0; i < info->policies_length; i++)
    {
        cJSON *policy = cJSON_GetArrayItem(policies, i);
        if (cJSON_IsObject(policy))
        {
            cJSON *priority = cJSON_GetObjectItemCaseSensitive(policy, "priority");
            cJSON *disabled = cJSON_GetObjectItemCaseSensitive(policy, "disabled");
            cJSON *min = cJSON_GetObjectItemCaseSensitive(policy, "min");
            cJSON *max = cJSON_GetObjectItemCaseSensitive(policy, "max");
            cJSON *batch_size = cJSON_GetObjectItemCaseSensitive(policy, "batch_size");

            if (!cJSON_IsNumber(priority) || !cJSON_IsBool(disabled) || !cJSON_IsNumber(min) || !cJSON_IsNumber(max) || !cJSON_IsNumber(batch_size))
            {
                ESP_LOGE(TAG, "Failed to read respawn counter info file policies");
                cJSON_Delete(json);
                return NULL;
            }

            info->policies[i].priority = priority->valueint;
            info->policies[i].disabled = cJSON_IsTrue(disabled);
            info->policies[i].min = min->valueint;
            info->policies[i].max = max->valueint;
            info->policies[i].batch_size = batch_size->valueint;
        }
    }

    cJSON_Delete(json);

    return info;
}

void sd_card_service_t::write_respawn_counter_info(respawn_counter_info_t *info)
{
    FILE *file = fopen(respawn_counter_info_file_path, "w");
    if (!file)
    {
        ESP_LOGW(TAG, "Failed to open respawn counter info file for writing");
        return;
    }

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "current_respawn_tokens", info->current_respawn_tokens);
    cJSON_AddNumberToObject(json, "respawn_tokens", info->respawn_tokens);

    cJSON *policies = cJSON_AddArrayToObject(json, "policies");
    for (uint8_t i = 0; i < info->policies_length; i++)
    {
        cJSON *policy = cJSON_CreateObject();
        cJSON_AddNumberToObject(policy, "priority", info->policies[i].priority);
        cJSON_AddBoolToObject(policy, "disabled", info->policies[i].disabled);
        cJSON_AddNumberToObject(policy, "min", info->policies[i].min);
        cJSON_AddNumberToObject(policy, "max", info->policies[i].max);
        cJSON_AddNumberToObject(policy, "batch_size", info->policies[i].batch_size);
        cJSON_AddItemToArray(policies, policy);
    }
    cJSON_AddNumberToObject(json, "policies_length", info->policies_length);

    const char *json_string = cJSON_Print(json);
    fprintf(file, json_string);
    fclose(file);

    cJSON_Delete(json);
    delete[] json_string;
}