idf_component_register(SRCS "main.c"
        "setup_webserver.c"
        "setup_wifi.c"
        "task_blink.c"        
    INCLUDE_DIRS "."
    REQUIRES freertos driver nvs_flash esp_wifi esp_https_server json)