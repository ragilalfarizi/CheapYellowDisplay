idf_component_register(SRCS "CheapYellowDisplay.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver
                             freertos
                             esp_lcd
)
