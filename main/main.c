#include "CYD_SDCard.h"
#include "CYD_UI.h"
#include "CYD_display.h"

static const char *TAG = "main";

// screen_t current_screen = STARTUP;

static void      slider_event_cb(lv_event_t *e);
static lv_obj_t *slider_label;

void lv_example_slider_1(void) {
  /*Create a slider in the center of the display*/
  lv_obj_t *slider = lv_slider_create(lv_screen_active());
  lv_obj_center(slider);
  lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  lv_obj_set_style_anim_duration(slider, 2000, 0);
  /*Create a label below the slider*/
  slider_label = lv_label_create(lv_screen_active());
  lv_label_set_text(slider_label, "0%");

  lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

static void slider_event_cb(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  char      buf[8];
  lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
  lv_label_set_text(slider_label, buf);
  lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

static void dd_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t       *obj  = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    LV_LOG_USER("Option: %s", buf);
  }
}

void lv_example_dropdown_1(void) {
  /*Create a normal drop down list*/
  lv_obj_t *dd = lv_dropdown_create(lv_screen_active());
  lv_dropdown_set_options(dd,
                          "Apple\n"
                          "Banana\n"
                          "Orange\n"
                          "Cherry\n"
                          "Grape\n"
                          "Raspberry\n"
                          "Melon\n"
                          "Orange\n"
                          "Lemon\n"
                          "Nuts");

  lv_obj_align(dd, LV_ALIGN_TOP_MID, 0, 20);
  lv_obj_add_event_cb(dd, dd_event_handler, LV_EVENT_ALL, NULL);
}

void app_main(void) {
  lv_init();

  /* LCD HW initialization */
  ESP_LOGI(TAG, "Entering LCD_INIT");
  LCD_init();

  // SDCard_init();

  // /* Touch initialization */
  ESP_LOGI(TAG, "Entering touch_init");
  touch_init();

  // /* LVGL initialization */
  ESP_LOGI(TAG, "Entering app_lvgl_init");
  app_lvgl_init();

  // /* Screen initialization */
  // ESP_LOGI(TAG, "Entering startup screen");
  // switch_screen(current_screen);

  // ESP_LOGI(TAG, "Entering demo_screen");
  // lv_example_slider_1();
  // lv_example_dropdown_1();
  // create_startup_screen();
  // vTaskDelay(pdMS_TO_TICKS(2000));

  setup_screens();

  while (1) {
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
