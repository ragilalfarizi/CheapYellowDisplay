idf_component_register(SRCS 
                    # main sources for component
                    "CYD_UI.c"
                    "CYD_home_screen.c"
                    "CYD_search_screen.c"
                    "CYD_setting_screen.c"
                    # images for screen and icon
                    "images/boot_screen_img.c"
                    "images/home_icon.c"
                    "images/search_icon.c"
                    "images/setting_icon.c"
                    "images/icon_bell.c"
                    "images/icon_radar.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos
)

set(EXTRA_COMPONENT_DIRS images) # this actually doesn't do anything.
# TODO: find a better way to glob the images folder
