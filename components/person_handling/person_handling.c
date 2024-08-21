#include "person_handling.h"

static const char      *TAG          = "Person Handler";
static uint8_t          current_size = 0;
i2c_master_dev_handle_t dev_handle;

esp_err_t i2c_init(void) {
  /* INSTALL I2C MASTER BUS CONFIG */
  i2c_master_bus_config_t i2c_mst_config = {
      .clk_source                   = I2C_CLK_SRC_DEFAULT,
      .i2c_port                     = I2C_NUM_0,
      .scl_io_num                   = PIN_I2C_SCL,
      .sda_io_num                   = PIN_I2C_SDA,
      .glitch_ignore_cnt            = 7,
      .flags.enable_internal_pullup = true,
  };

  i2c_master_bus_handle_t bus_handle;
  ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

  i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_7,
      .device_address  = I2C_SLAVE_ADDR,
      .scl_speed_hz    = 100000,
  };

  ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
  ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x08, -1));
  ESP_LOGI(TAG, "MASTER PROBE SUCCESSFUL");

  return ESP_OK;
}

// NOTE: maybe depreciated since we use queue eventually
esp_err_t allocate_person_dynamically(Person_t **person, uint8_t *capacity) {
  *person = malloc(*capacity * sizeof(Person_t));
  if (person == NULL) {
    ESP_LOGE(TAG, "Failed to allocate memory for persons array");
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "Succesfully created dynamic allocation for Person");
  return ESP_OK;
}

// NOTE: maybe depreciated since we use queue eventually
esp_err_t add_person(Person_t **persons, Person_t *new_person,
                     uint8_t *current_size) {
  // TODO: check

  (*persons)[*current_size] = *new_person;
  (*current_size)++;
  // // do something
  // if (*size >= *capacity) {
  //   *capacity *= 2;
  //   Person_t *temp = realloc(*persons, *capacity * sizeof(Person_t));
  //   if (temp == NULL) {
  //     ESP_LOGE(TAG, "Failed to reallocate memory");
  //     return ESP_FAIL;
  //   }
  //   *persons = temp;
  // }
  //
  // // add new person
  // (*persons)[*size].id = id;
  // strncpy((*persons)[*size].name, name, sizeof((*persons)[*size].name) - 1);
  // (*persons)[*size].name[sizeof((*persons)[*size].name) - 1] =
  //     '\0';  // Ensure null-termination
  // (*persons)[*size].pos_x = pos_x;
  // (*persons)[*size].pos_y = pos_y;
  // (*size)++;
  //
  return ESP_OK;
}

// NOTE: maybe depreciated since we use queue eventually
esp_err_t delete_person(Person_t **persons, int *size, int id) {
  for (int i = 0; i < *size; i++) {
    if ((*persons)[i].id == id) {
      // Shift the remaining elements
      for (int j = i; j < *size - 1; j++) {
        (*persons)[j] = (*persons)[j + 1];
      }

      // Decrease the size
      (*size)--;

      // Optionally, resize the array down if a lot of space is unused
      // (not necessary for the functionality, but can be done for efficiency)
    }
  }

  return ESP_OK;
}

Person_t deserialize_person(jparse_ctx_t *jctx, const char *json) {
  Person_t new_p;

  char name_temp[50];
  int  id_temp, pos_x_temp, pos_y_temp;

  ESP_ERROR_CHECK(json_parse_start(jctx, json, strlen(json)));

  json_obj_get_string(jctx, "name", name_temp, sizeof(name_temp));
  json_obj_get_int(jctx, "id", &id_temp);
  json_obj_get_int(jctx, "pos_x", &pos_x_temp);
  json_obj_get_int(jctx, "pos_y", &pos_y_temp);

  // Assigning values to new_p
  new_p.id = (uint8_t)id_temp;
  strncpy(new_p.name, name_temp, sizeof(new_p.name) - 1);
  new_p.name[sizeof(new_p.name) - 1] = '\0';  // Ensure null-termination
  new_p.pos_x                        = (int16_t)pos_x_temp;
  new_p.pos_y                        = (int16_t)pos_y_temp;

  json_parse_end(jctx);

  return new_p;
}
