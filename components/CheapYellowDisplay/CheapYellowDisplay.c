#include "CheapYellowDisplay.h"

static const char *TAG = "LCD";

esp_lcd_panel_io_handle_t io_handle    = NULL;
esp_lcd_panel_handle_t    panel_handle = NULL;
esp_lcd_touch_handle_t    tp           = NULL;

/* LVGL display and touch */
lv_display_t      *lvgl_disp        = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;

esp_err_t LCD_init(void) {
  ESP_LOGI(TAG, "Turn off LCD backlight");
  const gpio_config_t bk_gpio_config = {
      .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = 1ULL << TFT_BACKLIGHT_PIN};
  ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

  // 1. Create an SPI Bus
  ESP_LOGI(TAG, "Initialize SPI Bus");

  const spi_bus_config_t buscfg = {
      .sclk_io_num     = TFT_SCLK_PIN,
      .mosi_io_num     = TFT_MOSI_PIN,
      .miso_io_num     = TFT_MISO_PIN,
      .quadhd_io_num   = -1,  // not used
      .quadwp_io_num   = -1,  // not used
      .max_transfer_sz = TFT_H_RES * 80 * sizeof(uint16_t),
  };
  ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));

  // 2. Allocate an LCD IO Device handle from the SPI Bus
  ESP_LOGI(TAG, "Allocating LCD IO Device handle");

  const esp_lcd_panel_io_spi_config_t io_config = {
      .dc_gpio_num       = TFT_DC_PIN,
      .cs_gpio_num       = TFT_CS_PIN,
      .pclk_hz           = 40 * 1000 * 1000,
      .lcd_cmd_bits      = 8,
      .lcd_param_bits    = 8,
      .spi_mode          = 0,
      .trans_queue_depth = 10,
  };
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI3_HOST,
                                           &io_config, &io_handle));

  // 3. install LCD Controller Driver
  ESP_LOGI(TAG, "Installing LCD Controller Driver");

  const esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = TFT_RST_PIN,
      .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_BGR,
      .bits_per_pixel = 16,
  };

  ESP_ERROR_CHECK(
      esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, false));

  // user can flush pre-defined pattern to the screen before we turn on the
  // screen or backlight
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

  ESP_LOGI(TAG, "Turn on LCD backlight");
  gpio_set_level(TFT_BACKLIGHT_PIN, 1);

  return ESP_OK;
}

// FIX: this still doesn't work
esp_err_t touch_init(void) {
  static esp_lcd_panel_io_handle_t    tp_io_handle = NULL;
  const esp_lcd_panel_io_spi_config_t tp_io_config =
      ESP_LCD_TOUCH_IO_SPI_XPT2046_CONFIG(XPT2046_CS_PIN);
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI3_HOST,
                                           &tp_io_config, &tp_io_handle));

  const esp_lcd_touch_config_t tp_cfg = {
      .x_max        = TFT_H_RES,
      .y_max        = TFT_V_RES,
      .rst_gpio_num = -1,
      .int_gpio_num = XPT2046_IRQ_PIN,
      .flags =
          {
              .swap_xy  = 0,
              .mirror_x = 1,
              .mirror_y = 0,
          },
  };

  ESP_LOGI(TAG, "Initialize touch controller XPT2046");
  ESP_ERROR_CHECK(esp_lcd_touch_new_spi_xpt2046(tp_io_handle, &tp_cfg, &tp));

  return ESP_OK;
}

esp_err_t app_lvgl_init(void) {
  /* Initialize LVGL */
  const lvgl_port_cfg_t lvgl_cfg = {
      .task_priority = 4,    /* LVGL task priority */
      .task_stack    = 4096, /* LVGL task stack size */
      .task_affinity = -1,   /* LVGL task pinned to core (-1 is no affinity) */
      .task_max_sleep_ms = 500, /* Maximum sleep in LVGL task */
      .timer_period_ms   = 5    /* LVGL timer tick period in ms */
  };
  ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG,
                      "LVGL port initialization failed");

  /* Add LCD screen */
  ESP_LOGD(TAG, "Add LCD screen");
  const lvgl_port_display_cfg_t disp_cfg = {
      .io_handle     = io_handle,
      .panel_handle  = panel_handle,
      .buffer_size   = TFT_H_RES * 50,
      .double_buffer = 1,
      .hres          = TFT_H_RES,
      .vres          = TFT_V_RES,
      .monochrome    = false,
      .color_format  = LV_COLOR_FORMAT_RGB565,
      .rotation =
          {
              .swap_xy  = false,
              .mirror_x = false,
              .mirror_y = true,
          },
      .flags = {
          .buff_dma   = true,
          .swap_bytes = true,
      }};
  lvgl_disp = lvgl_port_add_disp(&disp_cfg);

  /* Add touch input (for selected screen) */
  // const lvgl_port_touch_cfg_t touch_cfg = {
  //     .disp   = lvgl_disp,
  //     .handle = tp,
  // };
  // lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

  lv_disp_set_rotation(lvgl_disp, LV_DISPLAY_ROTATION_180);

  return ESP_OK;
}
