#ifndef CYD_SDCARD_H_
#define CYD_SDCARD_H_

/* ========== MicroSD PIN CONNECTION ========== */
// The microSD card uses SPI communication protocol.
// It uses the ESP32 default VSPI pins:
#define MICROSD_MISO_PIN 19
#define MICROSD_MOSI_PIN 23
#define MICROSD_SCK_PIN  18
#define MICROSD_CS_PIN   5

#define MOUNT_POINT "/sdcard"

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

esp_err_t SDCard_init(void);

#endif  // !CYD_SDCARD_H_
