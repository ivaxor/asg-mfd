#ifndef __WIFI_CAPTIVE_PORTAL_ESP_IDF_COMPONENT_WIFI_CAPTIVE_PORTAL_ESP_IDF_HTTPD_H_INCLUDED__
#define __WIFI_CAPTIVE_PORTAL_ESP_IDF_COMPONENT_WIFI_CAPTIVE_PORTAL_ESP_IDF_HTTPD_H_INCLUDED__

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_event_base.h"

extern SemaphoreHandle_t xMutexHTTPD;

/** An event base type for "wifi-captive-portal-esp-idf-httpd". */
ESP_EVENT_DECLARE_BASE(WIFI_CAPTIVE_PORTAL_ESP_IDF_HTTPD_EVENT);
enum /**< The events. */
{
  WIFI_CAPTIVE_PORTAL_ESP_IDF_HTTPD_EVENT_FINISH, /**< An event for "this task is finished". */
};

/** The event loop handle. */
extern esp_event_loop_handle_t wifi_captive_portal_esp_idf_httpd_event_loop_handle;

#ifdef __cplusplus
extern "C"
{
#endif

  /** The esp-idf task function. */
  void wifi_captive_portal_esp_idf_httpd_task(void *pvParameter);

#ifdef __cplusplus
}
#endif

#endif
