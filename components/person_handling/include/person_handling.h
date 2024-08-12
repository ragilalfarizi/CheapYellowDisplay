#ifndef PERSON_HANDLING_H_
#define PERSON_HANDLING_H_

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "json_parser.h"
#include "string.h"

#define INITIAL_PERSON_CAPACITY 4
#define NAME_LENGTH             50
#define MAX_PERSON_COUNT        4

#define PIN_I2C_SCL    22
#define PIN_I2C_SDA    21
#define I2C_SLAVE_ADDR 0x58

typedef struct {
  uint16_t id;
  char     name[50];
  int16_t  pos_x;
  int16_t  pos_y;
} Person_t;

// extern Person_t *person_array;

esp_err_t i2c_init(void);

esp_err_t allocate_person_dynamically(Person_t **person, uint8_t *capacity);

esp_err_t add_person(Person_t **persons, Person_t *new_person,
                     uint8_t *current_size);

esp_err_t delete_person(Person_t **persons, int *size, int id);

// TODO: delete this
// void update_radar(SemaphoreHandle_t display_mutex, size_t *person_count,
//                   Person_t **person_array);
//
// void refresh_radar(void);
// Person_t get_person_from_i2c();

// Person_t serialize(const char* json);
//
Person_t deserialize_person(jparse_ctx_t *jctx, const char *json);

#endif  // !PERSON_HANDLING_H_
