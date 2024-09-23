#include "person_handling.h"

i2c_master_dev_handle_t dev_handle;

/******************************************************************************
 * START OF WORK IN PROGRESS
 *****************************************************************************/

static const char *TAG                = "BLE_Client";
static const char *remote_device_name = "bleUWB";

#define SERVICE_UUID                                                        \
  BLE_UUID128_DECLARE(0x4b, 0x91, 0x31, 0xc3, 0xc9, 0xc5, 0xcc, 0x8f, 0x9e, \
                      0x45, 0xb5, 0x1f, 0x01, 0xc2, 0xaf, 0x4f)
#define CHARACTERISTIC_UUID_1                                               \
  BLE_UUID128_DECLARE(0xa8, 0x26, 0x1b, 0x36, 0x07, 0xea, 0xf5, 0xb7, 0x88, \
                      0x46, 0xe1, 0x36, 0x3e, 0x48, 0xb5, 0xbe)
#define CHARACTERISTIC_UUID_2                                               \
  BLE_UUID128_DECLARE(0x7e, 0xe8, 0x7b, 0x5d, 0x2e, 0x7a, 0x3d, 0xbf, 0x3a, \
                      0x41, 0xf7, 0xd8, 0xe3, 0xd5, 0x95, 0x1c)

static uint8_t  ble_addr_type;
static uint16_t conn_handle;
static uint16_t service_handle;
static uint16_t characteristic_handle_1;
static uint16_t characteristic_handle_2;

static int mtu_exchange_cb(uint16_t                     conn_handle,
                           const struct ble_gatt_error *error, void *arg);

static void characteristic_read_task(void *param);

static int ble_on_disc_chr(uint16_t                     conn_handle,
                           const struct ble_gatt_error *error,
                           const struct ble_gatt_chr *chr, void *arg);

static int ble_gap_event(struct ble_gap_event *event, void *arg);

static int ble_on_notify(struct ble_gap_event *event, void *arg);

static int ble_on_write_cb(uint16_t                     conn_handle,
                           const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr, void *arg);

// Service discovery callback with correct function signature
static int ble_on_disc_svc(uint16_t                     conn_handle,
                           const struct ble_gatt_error *error,
                           const struct ble_gatt_svc *svc, void *arg) {
  ESP_LOGI(TAG, "entering ble_gatt_disc_svc_fn");
  if (error->status == 0) {
    // Buffer to store the UUID as a string (UUIDs can be up to 36 characters)
    char uuid_str[BLE_UUID_STR_LEN];

    // Convert the UUID to a string
    ble_uuid_to_str(&svc->uuid.u, uuid_str);

    // Log the UUID and handle range
    ESP_LOGI(TAG, "Discovered service with UUID: %s", uuid_str);
    ESP_LOGI(TAG, "Service handles: %d to %d", svc->start_handle,
             svc->end_handle);

    // Start discovering characteristics with the correct handle range
    int rc = ble_gattc_disc_all_chrs(conn_handle, svc->start_handle,
                                     svc->end_handle, ble_on_disc_chr, NULL);
    if (rc != 0) {
      ESP_LOGE(TAG, "Characteristic discovery failed with rc = %d", rc);
    }

  } else {
    ESP_LOGE(TAG, "Service discovery failed with status: %d", error->status);
  }
  return 0;
}

// Characteristic discovery callback with correct function signature
static int ble_on_disc_chr(uint16_t                     conn_handle,
                           const struct ble_gatt_error *error,
                           const struct ble_gatt_chr *chr, void *arg) {
  char uuid_str[BLE_UUID_STR_LEN];  // buffer to hold the string representation
                                    // of UUID
  if (error->status == 0) {
    // Convert the characteristic's UUID to string
    ble_uuid_to_str(&chr->uuid.u, uuid_str);
    ESP_LOGI(TAG, "Discovered characteristic with UUID: %s", uuid_str);

    // Depending on the UUID, assign the handle to the correct variable
    if (ble_uuid_cmp(&chr->uuid.u, CHARACTERISTIC_UUID_1)) {
      ESP_LOGI(TAG, "Found CHARACTERISTIC_UUID_1");
      characteristic_handle_1 = chr->def_handle;

      if (chr->properties & BLE_GATT_CHR_PROP_NOTIFY) {
        ESP_LOGI(TAG, "Characteristic supports notifications");
      }

    } else if (ble_uuid_cmp(&chr->uuid.u, CHARACTERISTIC_UUID_2)) {
      ESP_LOGI(TAG, "Found CHARACTERISTIC_UUID_2");
      characteristic_handle_2 = chr->def_handle;

      if (chr->properties & BLE_GATT_CHR_PROP_NOTIFY) {
        ESP_LOGI(TAG, "Characteristic supports notifications");
      }
    }

    /* SUBSCRIBE FUNCTION. */
    // Handle notifications using write_flat
    /*uint8_t notification_value[2] = {0x01, 0x00};  // Enable notifications*/
    /*int     rc                    = ble_gattc_write_flat(*/
    /*    conn_handle,*/
    /*    characteristic_handle_2 +*/
    /*        1,  // Assuming CCCD is right after the characteristic*/
    /*    notification_value, sizeof(notification_value), ble_on_write_cb,
     * NULL);*/
    /**/
    /*if (rc != 0) {*/
    /*  ESP_LOGE(TAG, "Failed to write to CCCD to enable notifications,
     * rc=%d",*/
    /*           rc);*/
    /*} else {*/
    /*  ESP_LOGI(TAG, "Subscribed to notifications on CHARACTERISTIC_UUID_2");*/
    /*}*/
  } else {
    ESP_LOGE(TAG, "Characteristic discovery failed with status: %d",
             error->status);
  }
  return 0;
}

static int ble_on_write_cb(uint16_t                     conn_handle,
                           const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr, void *arg) {
  if (error->status == 0) {
    printf("Write successful!\n");
  } else {
    printf("Write failed with error: %d\n", error->status);
  }
  return 0;
}

static int ble_on_notify(struct ble_gap_event *event, void *arg) {
  // Check if it's a notification (not an indication)
  if (event->notify_rx.indication == 0) {
    ESP_LOGI(TAG, "Notification received from characteristic handle: %d",
             event->notify_rx.attr_handle);

    static char data_buffer[512];
    static int  buffer_offset = 0;

    struct os_mbuf *om =
        event->notify_rx.om;            // Get the data from the notification
    uint16_t len = OS_MBUF_PKTLEN(om);  // Get the length of the data

    // Buffer to store the data
    char received_data[len + 1];        // +1 for null termination
    memset(received_data, 0, len + 1);  // Clear the buffer

    // Copy the data from the os_mbuf to our buffer
    ble_hs_mbuf_to_flat(om, received_data, sizeof(received_data), NULL);

    // Log the received data
    ESP_LOGI(TAG, "Received notification: %s", received_data);
  }
  return 0;
}

// Function to discover services and characteristics
static void discover_services_and_characteristics() {
  // Discover services
  int rc = ble_gattc_disc_svc_by_uuid(conn_handle, SERVICE_UUID,
                                      ble_on_disc_svc, NULL);
  if (rc != 0) {
    ESP_LOGE(TAG, "Service discovery failed with rc = %d", rc);
    return;
  }
}

// Callback function for BLE events (connected, disconnected, etc.)
static int ble_gap_event(struct ble_gap_event *event, void *arg) {
  int                      rc;
  struct ble_hs_adv_fields fields;

  switch (event->type) {
    case BLE_GAP_EVENT_DISC:
      rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                   event->disc.length_data);
      if (rc != 0) {
        return 0;
      }

      // Connect to determined ble server
      if (fields.name_len > 0) {
        ESP_LOGI(TAG, "Name: %.*s\n", fields.name_len, fields.name);

        // Check if the discovered device name matches the target device name
        if (strncmp((char *)fields.name, remote_device_name, fields.name_len) ==
            0) {
          printf("Found target device: %s\n", remote_device_name);

          // Stop scanning
          ble_gap_disc_cancel();

          // Initiate connection to the device
          rc = ble_gap_connect(BLE_OWN_ADDR_PUBLIC, &event->disc.addr,
                               BLE_HS_FOREVER, NULL, ble_gap_event, NULL);
          if (rc != 0) {
            printf("Error: Failed to connect to device, rc=%d\n", rc);
          }
        }
      }
      break;

    case BLE_GAP_EVENT_CONNECT:
      if (event->connect.status == 0) {
        ESP_LOGI(TAG, "Connected to device!");
        conn_handle = event->connect.conn_handle;

        ble_gattc_exchange_mtu(event->connect.conn_handle, mtu_exchange_cb,
                               NULL);

        // Search for the services and characteristics
        discover_services_and_characteristics();

      } else {
        ESP_LOGI(TAG, "Failed to connect, status = %d", event->connect.status);
      }
      break;

    case BLE_GAP_EVENT_DISCONNECT:
      ESP_LOGI(TAG, "Disconnected from device");
      break;

    case BLE_GAP_EVENT_NOTIFY_RX:
      ble_on_notify(event, arg);
      break;

    default:
      break;
  }
  return 0;
}

static int mtu_exchange_cb(uint16_t                     conn_handle,
                           const struct ble_gatt_error *error, void *arg) {
  if (error->status == 0) {
    ESP_LOGI("BLE", "MTU exchange successful, MTU: %d",
             ble_att_mtu(conn_handle));
  } else {
    ESP_LOGE("BLE", "MTU exchange failed, error code: %d", error->status);
  }
  return 0;
}

// Function to initiate BLE scanning
void start_ble_scan(void) {
  int rc;

  // Determine the BLE address type (public or random)
  rc = ble_hs_id_infer_auto(0, &ble_addr_type);
  if (rc != 0) {
    ESP_LOGE(TAG, "Error determining address type; rc=%d", rc);
    return;
  }

  struct ble_gap_disc_params disc_params;
  memset(&disc_params, 0, sizeof(disc_params));
  disc_params.itvl          = BLE_GAP_SCAN_FAST_INTERVAL_MIN;
  disc_params.window        = BLE_GAP_SCAN_FAST_WINDOW;
  disc_params.filter_policy = BLE_HCI_SCAN_FILT_NO_WL;
  disc_params.passive       = 0;

  rc = ble_gap_disc(ble_addr_type, BLE_HS_FOREVER, &disc_params, ble_gap_event,
                    NULL);
  if (rc != 0) {
    ESP_LOGE(TAG, "Error initiating scan; rc=%d", rc);
  } else {
    ESP_LOGI(TAG, "BLE scan started successfully");
  }
}

void ble_host_task(void *param) {
  // This function will run the BLE host
  nimble_port_run();              // This function should never return
  nimble_port_freertos_deinit();  // Clean up if the host exits
}

void ble_init(void) {
  // Initialize NVS (required for BLE)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
  }

  // Initialize BLE client
  nimble_port_init();
  int rc = ble_att_set_preferred_mtu(64);  // Set larger MTU size
  if (rc != 0) {
    ESP_LOGE(TAG, "The MTU is not 64");
  }
  ble_hs_cfg.sync_cb = start_ble_scan;

  /* Set the default device name. */
  ble_svc_gap_device_name_set("nimble-blecent");
  nimble_port_freertos_init(ble_host_task);
}
/******************************************************************************
 * END OF WORK IN PROGRESS
 *****************************************************************************/

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

/*// NOTE: maybe depreciated since we use queue eventually*/
/*esp_err_t allocate_person_dynamically(Person_t **person, uint8_t *capacity)
 * {*/
/*  *person = malloc(*capacity * sizeof(Person_t));*/
/*  if (person == NULL) {*/
/*    ESP_LOGE(TAG, "Failed to allocate memory for persons array");*/
/*    return ESP_FAIL;*/
/**/
/*    ESP_LOGI(TAG, "Succesfully created dynamic allocation for Person");*/
/*    return ESP_OK;*/
/*  }*/
/*}*/
/**/
/*// NOTE: maybe depreciated since we use queue eventually*/
/*esp_err_t add_person(Person_t **persons, Person_t *new_person,*/
/*                     uint8_t *current_size) {*/
/*  // TODO: check*/
/**/
/*  (*persons)[*current_size] = *new_person;*/
/*  (*current_size)++;*/
/*  // // do something*/
/*  // if (*size >= *capacity) {*/
/*  //   *capacity *= 2;*/
/*  //   Person_t *temp = realloc(*persons, *capacity * sizeof(Person_t));*/
/*  //   if (temp == NULL) {*/
/*  //     ESP_LOGE(TAG, "Failed to reallocate memory");*/
/*  //     return ESP_FAIL;*/
/*  //   }*/
/*  //   *persons = temp;*/
/*  // }*/
/*  //*/
/*  // // add new person*/
/*  // (*persons)[*size].id = id;*/
/*  // strncpy((*persons)[*size].name, name, sizeof((*persons)[*size].name)
 * -*/
/*  // 1);*/
/*  // (*persons)[*size].name[sizeof((*persons)[*size].name) - 1] =*/
/*  //     '\0';  // Ensure null-termination*/
/*  // (*persons)[*size].pos_x = pos_x;*/
/*  // (*persons)[*size].pos_y = pos_y;*/
/*  // (*size)++;*/
/*  //*/
/*  return ESP_OK;*/
/*}*/
/**/
/*// NOTE: maybe depreciated since we use queue eventually*/
/*esp_err_t delete_person(Person_t **persons, int *size, int id) {*/
/*  for (int i = 0; i < *size; i++) {*/
/*    if ((*persons)[i].id == id) {*/
/*      // Shift the remaining elements*/
/*      for (int j = i; j < *size - 1; j++) {*/
/*        (*persons)[j] = (*persons)[j + 1];*/
/*      }*/
/**/
/*      // Decrease the size*/
/*      (*size)--;*/
/**/
/*      // Optionally, resize the array down if a lot of space is unused*/
/*      // (not necessary for the functionality, but can be done for
 * efficiency)*/
/*    }*/
/*  }*/
/**/
/*  return ESP_OK;*/
/*}*/

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
