idf_component_register(SRCS "person_handling.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver
                    freertos
                    espressif__json_parser
                    bt
                    nvs_flash
)
