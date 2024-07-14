#include "CheapYellowDisplay.h"

void app_main(void) {
  /* LCD HW initialization */
  LCD_init();

  /* Touch initialization */
  touch_init();

  /* LVGL initialization */
  app_lvgl_init();

  /* Show LVGL objects */
  // app_main_display();
}
