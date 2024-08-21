#ifndef CYD_UI_H_
#define CYD_UI_H_

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "lvgl.h"

#define COLOR_PRIMARY   lv_color_hex(0x222831)
#define COLOR_SECONDARY lv_color_hex(0x31363F)
#define COLOR_TERTIARY  lv_color_hex(0x76ABAE)
#define COLOR_ACCENT    lv_color_hex(0xEEEEEE)

typedef enum {
  STARTUP,
  HOME,
  SEARCH,
  SETTINGS,
} screen_id_t;

extern TaskHandle_t radar_display_handler;
extern TaskHandle_t add_rand_person_handler;

void      setup_screens();
void      create_startup_screen();
lv_obj_t *create_screen(lv_color_t color);
lv_obj_t *create_search_screen();
lv_obj_t *create_setting_screen();
void create_bottom_menu(lv_obj_t *parent);
void create_top_bar(lv_obj_t *parent);

#endif  // !CYD_UI_H_
