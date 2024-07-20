#include "CheapYellowDisplay.h"
#include "UI_manager.h"

static void _app_button_cb(lv_event_t *e) {
  lv_disp_rotation_t rotation = lv_disp_get_rotation(lvgl_disp);
  rotation++;
  if (rotation > LV_DISPLAY_ROTATION_270) {
    rotation = LV_DISPLAY_ROTATION_0;
  }

  /* LCD HW rotation */
  lv_disp_set_rotation(lvgl_disp, rotation);
}

static void app_main_display(void) {
  lv_obj_t *scr = lv_scr_act();

  /* Task lock */
  lvgl_port_lock(0);

  /* Your LVGL objects code here .... */

  /* Label */
  lv_obj_t *label = lv_label_create(scr);
  lv_obj_set_width(label, TFT_H_RES);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
#if LVGL_VERSION_MAJOR == 8
  lv_label_set_recolor(label, true);
  lv_label_set_text(
      label, "#FF0000 " LV_SYMBOL_BELL
             " Hello world Espressif and LVGL " LV_SYMBOL_BELL
             "#\n#FF9400 " LV_SYMBOL_WARNING
             " For simplier initialization, use BSP " LV_SYMBOL_WARNING " #");
#else
  lv_label_set_text(label, LV_SYMBOL_BELL
                    " Hello world Espressif and LVGL " LV_SYMBOL_BELL
                    "\n " LV_SYMBOL_WARNING
                    " For simplier initialization, use BSP " LV_SYMBOL_WARNING);
#endif
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);

  /* Button */
  lv_obj_t *btn = lv_btn_create(scr);
  label         = lv_label_create(btn);
  lv_label_set_text_static(label, "Rotate screen");
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -30);
  lv_obj_add_event_cb(btn, _app_button_cb, LV_EVENT_CLICKED, NULL);

  /* Task unlock */
  lvgl_port_unlock();
}

void change_bg_color(lv_color_t color, const char *text, uint32_t delay) {
  lv_obj_t *scr = lv_screen_active();
  lv_obj_clean(scr);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_bg_color(&style, color);

  /* Apply the style to the screen */
  lv_obj_add_style(scr, &style, 0);

  /* Create a style for the text */
  static lv_style_t style_text;
  lv_style_init(&style_text);
  lv_style_set_text_color(&style_text, lv_color_white());

  /* Create a label and set its text */
  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text(label, text);
  lv_obj_add_style(label, &style_text, 0);  // Apply the text style to the label
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  vTaskDelay(pdMS_TO_TICKS(delay));
}

void app_main(void) {
  /* LCD HW initialization */
  LCD_init();

  SDCard_init();
  //
  // /* Touch initialization */
  // touch_init();
  //
  // /* LVGL initialization */
  // app_lvgl_init();
  //
  // lv_disp_set_rotation(lvgl_disp, LV_DISP_ROTATION_180);
  //
  // /* Show LVGL objects */
  // // app_main_display();
  //
  // while (1) {
  //   lv_task_handler();
  //
  //   change_bg_color((lv_color_hex(0xFF0000)), "Background is Red", 3000);
  //   change_bg_color((lv_color_hex(0x00FF00)), "Background is Green", 3000);
  //   change_bg_color((lv_color_hex(0x0000FF)), "Background is Blue", 3000);
  //   change_bg_color((lv_color_hex(0xFFFFFF)), "Background is White", 3000);
  //   change_bg_color((lv_color_hex(0x000000)), "Background is Black", 3000);
  // }
}
