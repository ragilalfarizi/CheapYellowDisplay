idf_component_register(SRCS "CYD_display.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver
                             freertos
                             esp_lcd
)
