#include "CYD_SDCard.h"
#include "CYD_UI.h"
#include "CYD_display.h"

static const char *TAG = "main";

typedef struct {
  char   *name;
  uint8_t id;
  int8_t  pos_x;
  int8_t  pos_y;
} Person_t;

// TODO: create dynamic allocation for Person Struct using dynamic array

void app_main(void) {
  /* LCD HW initialization */
  ESP_LOGI(TAG, "Entering LCD_INIT");
  LCD_init();

  /* LCD HW initialization */
  // SDCard_init();

  /* Touch initialization */
  ESP_LOGI(TAG, "Entering touch_init");
  touch_init();

  /* LVGL initialization */
  ESP_LOGI(TAG, "Entering app_lvgl_init");
  app_lvgl_init();
  lv_init();

  setup_screens();

  // TODO: migrate to RTOS
  // TODO: create Task
  //       - display task
  //       - i2c handling

  while (1) {
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
