idf_component_register(SRCS 
                    "CYD_UI.c"
                    "boot_screen.c"
                    "images/boot_screen_img.c"
                    "images/home_icon.c"
                    "images/search_icon.c"
                    "images/setting_icon.c"
                    "images/icon_bell.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos
)

set(EXTRA_COMPONENT_DIRS images )
