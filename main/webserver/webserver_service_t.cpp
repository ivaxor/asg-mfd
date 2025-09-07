#include "sys/param.h"
#include "lwip/inet.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "dns_server.h"
#include "include/webserver_service_t.h"
#include "include/webserver_static_handlers_t.h"
#include "include/webserver_game_mode_handlers_t.h"
#include "include/webserver_respawn_counter_handlers_t.h"

const char *webserver_service_t::TAG = "webserver_service_t";

void webserver_service_t::wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
}

void webserver_service_t::wifi_init_softap()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &webserver_service_t::wifi_event_handler, NULL));

    uint8_t mac_addr[6];
    char ap_ssid[32];
    esp_read_mac(mac_addr, ESP_MAC_WIFI_SOFTAP);
    snprintf(ap_ssid, sizeof(ap_ssid), "ASG-MFD-%02X%02X%02X%02X", mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "",
            .password = "",
            .ssid_len = (uint8_t)strlen(ap_ssid),
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 4,
        },
    };
    memcpy(wifi_config.ap.ssid, ap_ssid, sizeof(ap_ssid));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s'", ap_ssid);
}

void webserver_service_t::dhcp_set_captiveportal_url()
{
    // get the IP of the access point to redirect to
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);

    // turn the IP into a URI
    char *captiveportal_uri = (char *)malloc(32 * sizeof(char));
    assert(captiveportal_uri && "Failed to allocate captiveportal_uri");
    strcpy(captiveportal_uri, "http://");
    strcat(captiveportal_uri, ip_addr);

    // get a handle to configure DHCP with
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    // set the DHCP option 114
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_stop(netif));
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI, captiveportal_uri, strlen(captiveportal_uri)));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_start(netif));
}

httpd_handle_t webserver_service_t::start_webserver()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.max_uri_handlers = 32;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%lu'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");

        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_static_handlers_t::index_html_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_static_handlers_t::main_js_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_static_handlers_t::scripts_js_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_static_handlers_t::styles_css_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_static_handlers_t::favicon_ico_uri));

        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_game_mode_handlers_t::game_mode_info_options_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_game_mode_handlers_t::game_mode_info_get_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_game_mode_handlers_t::game_mode_info_post_uri));

        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_respawn_counter_handlers_t::respawn_counter_info_options_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_respawn_counter_handlers_t::respawn_counter_info_get_uri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webserver_respawn_counter_handlers_t::respawn_counter_info_post_uri));

        ESP_ERROR_CHECK(httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, webserver_static_handlers_t::http_404_error_handler));
    }
    return server;
}

void webserver_service_t::init()
{
    /*
        Turn of warnings from HTTP server as redirecting traffic will yield
        lots of invalid requests
    */
    esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
    esp_log_level_set("httpd_txrx", ESP_LOG_ERROR);
    esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

    // Initialize networking stack
    ESP_ERROR_CHECK(esp_netif_init());

    // Create default event loop needed by the  main app
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize NVS needed by Wi-Fi
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize Wi-Fi including netif with default config
    esp_netif_create_default_wifi_ap();

    // Initialise ESP32 in SoftAP mode
    wifi_init_softap();

    // Configure DNS-based captive portal
    dhcp_set_captiveportal_url();

    // Start the server for the first time
    start_webserver();

    // Start the DNS server that will redirect all queries to the softAP IP
    dns_server_config_t config = DNS_SERVER_CONFIG_SINGLE("*" /* all A queries */, "WIFI_AP_DEF" /* softAP netif ID */);
    start_dns_server(&config);
}