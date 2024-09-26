#ifndef PERSON_HANDLING_H_
#define PERSON_HANDLING_H_

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "json_parser.h"
#include "nvs_flash.h"
#include "string.h"

/* BLE */
#include "console/console.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
// #include "blecent.h"

#define INITIAL_PERSON_CAPACITY 4
#define NAME_LENGTH             50
#define MAX_PERSON_COUNT        4
#define SIZE_OF_BLE_MTU         64

#define PIN_I2C_SCL    22
#define PIN_I2C_SDA    27
#define I2C_SLAVE_ADDR 0x08

extern QueueHandle_t serialized_json_data_queue;

typedef struct {
  uint16_t id;
  char     name[50];
  int16_t  pos_x;
  int16_t  pos_y;
} Person_t;

void ble_init(void);

Person_t deserialize_person(jparse_ctx_t *jctx, const char *json);

/** NOTE:
 * Perhaps these functions need to be depreciated.
 * since person management is using queue.
 */
esp_err_t i2c_init(void);

#endif  // !PERSON_HANDLING_H_
