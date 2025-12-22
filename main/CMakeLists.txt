file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
file(GLOB_RECURSE EMBED_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "dist/browser/*.*")

idf_component_register(
    SRCS
        ${SRCS}
    REQUIRES
        "freertos"
        "driver"
        "nvs_flash"
        "esp_wifi"
        "esp_event"
        "esp_timer"
        "esp_http_server"
        "dns_server"
        "fatfs" # esp_vfs_fat
        "esp_driver_sdspi"
        "max7219"
        "esp_driver_spi" # max7219 dependency
        "esp_driver_gpio" # max7219 dependency
    EMBED_FILES
        ${EMBED_FILES})