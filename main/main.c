#include <stdlib.h>
#include <time.h>

#include "CYD_SDCard.h"
#include "CYD_UI.h"
#include "CYD_display.h"
#include "CYD_search_screen.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "person_handling.h"

/* STATIC PROTOTYPE DECLARATION */
void lvgl_display_task(void *pvParameter);
void I2C_data_handling_task(void *pvParameter);
void radar_display_task(void *pvParameter);
void add_rand_person_task(void *pvParameter);

/* TASK HANDLER DECLARATION */
TaskHandle_t lvgl_display_handler            = NULL;
TaskHandle_t i2c_data_handling_task_handler  = NULL;
TaskHandle_t radar_display_handler           = NULL;
TaskHandle_t add_rand_person_handler         = NULL;

// TODO: create dynamic allocation for Person Struct using dynamic array

/* GLOBAL VARIABLES */
static const char             *TAG            = "main";
volatile screen_id_t           current_screen = HOME;
uint8_t                        capacity       = INITIAL_PERSON_CAPACITY;
extern i2c_master_dev_handle_t dev_handle;

QueueHandle_t     serialized_json_data_queue;
QueueHandle_t     person_data_q;
SemaphoreHandle_t display_mutex;
SemaphoreHandle_t i2c_sem;

void app_main(void) {
  // TODO: change the size into the lenght of upcoming serialized json data
  serialized_json_data_queue = xQueueCreate(10, sizeof(char *));
  if (serialized_json_data_queue == NULL) {
    ESP_LOGE(TAG, "Failed to create serialized queue");
    return;
  }

  person_data_q = xQueueCreate(10, sizeof(Person_t));
  if (person_data_q == NULL) {
    ESP_LOGE(TAG, "Failed to create person queue");
    return;
  }

  display_mutex = xSemaphoreCreateMutex();
  i2c_sem       = xSemaphoreCreateBinary();

  /* LCD HW initialization */
  ESP_LOGI(TAG, "Entering LCD_INIT");
  LCD_init();

  /* LCD HW initialization */
  // SDCard_init();

  /* Touch initialization */
  ESP_LOGI(TAG, "Entering touch_init");
  touch_init();

  /* I2C initialization */
  // ESP_LOGI(TAG, "Entering I2C init");
  // i2c_init();

  /* LVGL initialization */
  ESP_LOGI(TAG, "Entering app_lvgl_init");
  app_lvgl_init();

  setup_screens();

  /* TASK CREATION */
  xTaskCreatePinnedToCore(lvgl_display_task, "LVGL Display Task", 4096, NULL, 5,
                          &lvgl_display_handler, 0);
  // xTaskCreatePinnedToCore(I2C_data_handling_task, "I2C Data Handling", 4096,
  //                         person_data_q, 3, &i2c_data_handling_task_handler,
  //                         1);
  xTaskCreatePinnedToCore(radar_display_task, "Radar Display Task", 3584,
                          person_data_q, 3, &radar_display_handler, 1);
  xTaskCreatePinnedToCore(add_rand_person_task, "add random person to queue",
                          2048, person_data_q, 3, &add_rand_person_handler, 1);

  vTaskSuspend(radar_display_handler);
  vTaskSuspend(add_rand_person_handler);
}

void lvgl_display_task(void *pvParameter) {
  lv_init();

  while (1) {
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void I2C_data_handling_task(void *pvParameter) {
  char data[128];

  while (1) {
    if (xSemaphoreTake(i2c_sem, portMAX_DELAY) == pdTRUE) {
      esp_err_t ret =
          i2c_master_receive(dev_handle, (uint8_t *)data, sizeof(data), -1);
      if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to receive data");
        continue;
      }

      printf("%s\n", data);

      if (xQueueSend(serialized_json_data_queue, &data, portMAX_DELAY) !=
          pdPASS) {
        ESP_LOGE("I2C Task", "Failed to send data to the queue");
      }
    }
  }
}

void radar_display_task(void *pvParameter) {
  ESP_LOGI(TAG, "Entering radar display task");

  QueueHandle_t person_queue = (QueueHandle_t)pvParameter;
  if (person_queue == NULL) {
    ESP_LOGE(TAG, "Failed to pass person queue to radar task");
    return;
  }

  Person_t person;

  while (1) {
    while (xQueueReceive(person_queue, &person, 0) == pdPASS) {
      draw_dot_info(&person);
    }

    // wait 3 seconds
    vTaskDelay(pdMS_TO_TICKS(3000));
    clear_radar_display();
  }
}

void add_rand_person_task(void *pvParameter) {
  QueueHandle_t person_queue = (QueueHandle_t)pvParameter;
  if (person_queue == NULL) {
    ESP_LOGE(TAG, "Failed to pass person queue to random person task");
    return;
  }

  Person_t person;

  // seed the random generator
  srand(time(NULL));

  while (1) {
    // generate random person data
    person.id = rand() % 1001;  // random ID between 0 and 1000
    snprintf(person.name, sizeof(person.name), "Person_%d", person.id);
    person.pos_x =
        (rand() & 201) - 100;  // random X position between -100 to 100
    person.pos_y =
        (rand() & 201) - 100;  // random Y position between -100 to 100

    // send person data to the queue
    if (xQueueSend(person_queue, &person, portMAX_DELAY) != pdPASS) {
      ESP_LOGE(TAG, "Failed to add person to the queue");
    }

    // delay between adding persons
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
