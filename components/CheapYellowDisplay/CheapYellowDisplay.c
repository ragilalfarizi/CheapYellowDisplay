#include "CheapYellowDisplay.h"

static const char *TAG = "LCD";

esp_err_t LCD_init(void) {
  ESP_LOGI(TAG, "Turn off LCD backlight");
  gpio_config_t bk_gpio_config = {.mode         = GPIO_MODE_OUTPUT,
                                  .pin_bit_mask = 1ULL << TFT_BACKLIGHT_PIN};
  ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

  // 1. Create an SPI Bus
  ESP_LOGI(TAG, "Initialize SPI Bus");

  spi_bus_config_t buscfg = {
      .sclk_io_num     = TFT_SCLK_PIN,
      .mosi_io_num     = TFT_MOSI_PIN,
      .miso_io_num     = TFT_MISO_PIN,
      .quadhd_io_num   = -1,  // not used
      .quadwp_io_num   = -1,  // not used
      .max_transfer_sz = TFT_H_RES * 80 * sizeof(uint16_t),
  };
  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

  // 2. Allocate an LCD IO Device handle from the SPI Bus
  ESP_LOGI(TAG, "Allocating LCD IO Device handle");

  esp_lcd_panel_io_handle_t     io_handle = NULL;
  esp_lcd_panel_io_spi_config_t io_config = {
      .dc_gpio_num       = TFT_DC_PIN,
      .cs_gpio_num       = TFT_CS_PIN,
      .pclk_hz           = 30 * 1000 * 1000,
      .lcd_cmd_bits      = 8,
      .lcd_param_bits    = 8,
      .spi_mode          = 0,
      .trans_queue_depth = 10,
  };
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                                           &io_config, &io_handle));

  // 3. install LCD Controller Driver
  ESP_LOGI(TAG, "Installing LCD Controller Driver");

  esp_lcd_panel_handle_t     panel_handle = NULL;
  esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = TFT_RST_PIN,
      .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_BGR,
      .bits_per_pixel = 18,
  };

  ESP_ERROR_CHECK(
      esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));

  // user can flush pre-defined pattern to the screen before we turn on the
  // screen or backlight
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

  ESP_LOGI(TAG, "Turn on LCD backlight");
  gpio_set_level(TFT_BACKLIGHT_PIN, 1);

  return ESP_OK;
}
