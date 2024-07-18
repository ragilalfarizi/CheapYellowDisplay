#include "UI_manager.h"

static const char *TAG = "SD-Card";

esp_err_t SDCard_init(void) {
  esp_err_t ret;

  // Initialize SD Card
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files              = 5,
      .allocation_unit_size   = 16 * 1024,
  };
  sdmmc_card_t *card;
  const char    mount_point[] = MOUNT_POINT;
  ESP_LOGI(TAG, "Initializing SD card");

  ESP_LOGI(TAG, "Using SPI peripheral");

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot         = SPI2_HOST;

  // Initialize SPI Bus
  spi_bus_config_t bus_cfg = {
      .mosi_io_num     = MICROSD_MOSI_PIN,
      .miso_io_num     = MICROSD_MISO_PIN,
      .sclk_io_num     = MICROSD_SCK_PIN,
      .quadhd_io_num   = -1,
      .quadwp_io_num   = -1,
      .max_transfer_sz = 4000,
  };

  ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize bus.");
    return ret;
  }

  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs               = MICROSD_CS_PIN;
  slot_config.host_id               = host.slot;

  ESP_LOGI(TAG, "Mounting filesystem");
  ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config,
                                &card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG,
               "Failed to mount filesystem. "
               "If you want the card to be formatted, set the "
               "CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(TAG,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
    return ret;
  }
  ESP_LOGI(TAG, "Filesystem mounted");

  return ret;
}
