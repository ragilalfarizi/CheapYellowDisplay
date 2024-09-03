#ifndef CYD_display_H_
#define CYD_display_H_

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_xpt2046.h"
#include "esp_lcd_types.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "indev/lv_indev.h"
#include "lvgl.h"

extern esp_lcd_panel_io_handle_t io_handle;
extern esp_lcd_panel_handle_t    panel_handle;
extern esp_lcd_touch_handle_t    tp;
extern esp_lcd_panel_io_handle_t tp_io_handle;
extern lv_display_t             *lvgl_disp;
extern lv_indev_t               *lvgl_touch_indev;
/* ========== SPI TFT PIN CONNECTION ========== */
// The TFT display communicates with the board using SPI communication protocol
// (HSPI)
#define TFT_MISO_PIN      12
#define TFT_MOSI_PIN      13
#define TFT_SCLK_PIN      14
#define TFT_CS_PIN        15
#define TFT_DC_PIN        2
#define TFT_RST_PIN       -1
#define TFT_BACKLIGHT_PIN 21
#define TFT_H_RES         240
#define TFT_V_RES         320

/* ========== TOUCHSCREEN PIN CONNECTION ========== */
// The touchscreen also uses SPI protocol to communicate with the ESP32.
// These are the VSPI pins for the touchscreen:
#define XPT2046_IRQ_PIN  36
#define XPT2046_MOSI_PIN 32
#define XPT2046_MISO_PIN 39
#define XPT2046_CLK_PIN  25
#define XPT2046_CS_PIN   33

/* ========== RGB LED PIN CONNECTION ========== */
// The RGB LEDs work with inverted logic, because they are active low.
// This means that if you set them to HIGH = OFF and LOW = ON.
#define LED_RED_PIN   4
#define LED_GREEN_PIN 16
#define LED_BLUE_PIN  17

// TODO: Move below to SD Card Components

/* ========== MicroSD PIN CONNECTION ========== */
// The microSD card uses SPI communication protocol.
// It uses the ESP32 default VSPI pins:
#define MICROSD_MISO_PIN 19
#define MICROSD_MOSI_PIN 23
#define MICROSD_SCK_PIN  18
#define MICROSD_CS_PIN   5

/* ========== LDR PIN CONNECTION ========== */
#define LDR_PIN 34

// TODO: add P3, CN1, and TX/RX pin connection

/* ================================================= */
esp_err_t LCD_init(void);
esp_err_t cyd_touch_init(void);
esp_err_t app_lvgl_init(void);

#endif  // !CHEAPYELLOWDISPLAY_H_
