#include "CYD_display.h"
#include "CYD_SDCard.h"
#include "CYD_UI.h"

LV_IMAGE_DECLARE(welcome);
LV_IMAGE_DECLARE(menu);
LV_IMAGE_DECLARE(radar);

static lv_obj_t* img_obj;

void create_image_example(void) {
  lv_obj_t* scr = lv_scr_act();
  img_obj       = lv_img_create(scr);
  lv_img_set_src(img_obj, &welcome);
  lv_obj_center(img_obj);
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
  app_lvgl_init();

  create_image_example();

  //
  //
  // /* Show LVGL objects */
  // // app_main_display();
  //
  while (1) {
    lv_img_set_src(img_obj, &menu);
    vTaskDelay(pdMS_TO_TICKS(3000));

    lv_img_set_src(img_obj, &radar);
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    lv_img_set_src(img_obj, &welcome);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}
