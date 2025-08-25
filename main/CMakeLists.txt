idf_component_register(SRCS "main.c" "blink_task.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos driver)