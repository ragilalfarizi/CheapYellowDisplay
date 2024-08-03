#ifndef CYD_UI_H_
#define CYD_UI_H_

#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"

extern lv_obj_t *home_screen;
extern lv_obj_t *search_screen;
extern lv_obj_t *setting_screen;

typedef enum {
  STARTUP,
  HOME,
  SEARCH,
  SETTINGS,
} screen_id_t;

void setup_screens();

static void menu_event_handler(lv_event_t *e);
static void create_bottom_menu(lv_obj_t *parent);
void create_startup_screen();
lv_obj_t *create_screen(lv_color_t color);

#endif  // !CYD_UI_H_
