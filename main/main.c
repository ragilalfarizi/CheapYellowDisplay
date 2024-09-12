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
void add_rand_person_task(void *pvParameter);
// void radar_display_task(void *pvParameter);

/* TASK HANDLER DECLARATION */
TaskHandle_t lvgl_display_handler    = NULL;
TaskHandle_t radar_display_handler   = NULL;
TaskHandle_t add_rand_person_handler = NULL;
TaskHandle_t BLE_handler             = NULL;

/* GLOBAL VARIABLES */
Person_t             person;
static const char   *TAG            = "main";
volatile screen_id_t current_screen = HOME;

QueueHandle_t     serialized_json_data_queue;
QueueHandle_t     person_data_q;
SemaphoreHandle_t display_mutex;

void app_main(void) {
  /**********************************************/
  /* BLUETOOTH LOW ENERGY INITIALIZATION START */
  ble_init();
  /**********************************************/
  /* BLUETOOTH LOW ENERGY INITIALIZATION DONE */

  // TODO: change the size into the lenght of upcoming serialized json data
  serialized_json_data_queue = xQueueCreate(5, sizeof(char *));
  if (serialized_json_data_queue == NULL) {
    ESP_LOGE(TAG, "Failed to create serialized queue");
    return;
  }

  person_data_q = xQueueCreate(5, sizeof(Person_t));
  if (person_data_q == NULL) {
    ESP_LOGE(TAG, "Failed to create person queue");
    return;
  }

  display_mutex = xSemaphoreCreateMutex();
  if (display_mutex == NULL) {
    ESP_LOGE(TAG, "Failed to create person queue");
  }

  /* LCD HW initialization */
  ESP_LOGI(TAG, "Entering LCD_INIT");
  LCD_init();

  /* LCD HW initialization */
  // SDCard_init();

  /* Touch initialization */
  ESP_LOGI(TAG, "Entering touch_init");
  cyd_touch_init();

  /* LVGL initialization */
  ESP_LOGI(TAG, "Entering app_lvgl_init");
  app_lvgl_init();

  setup_screens();

  /* TASK CREATION */

  xTaskCreatePinnedToCore(lvgl_display_task, "LVGL Display Task", 5120, NULL, 5,
                          &lvgl_display_handler, 0);
  // xTaskCreatePinnedToCore(radar_display_task, "Radar Display Task", 2048,
  // NULL,
  //                         3, &radar_display_handler, 1);
  ESP_LOGI(TAG, "creating rand_person task");
  xTaskCreatePinnedToCore(add_rand_person_task, "add random person to queue",
                          2048, person_data_q, 3, &add_rand_person_handler, 1);

  // vTaskSuspend(add_rand_person_handler);
}

void lvgl_display_task(void *pvParameter) {
  lv_init();

  while (1) {
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// OPTIMIZE: will be depreciated
void radar_display_task(void *pvParameter) {
  ESP_LOGI(TAG, "Entering radar display task");

  while (1) {
    // CREATE MUTEX
    if (xSemaphoreTake(display_mutex, portMAX_DELAY) == pdTRUE) {
      ESP_LOGI(TAG, "display mutex is available in main task.");
      if (xQueueReceive(person_data_q, &person, 0) == pdPASS) {
        draw_dot_info(&person);

        printf("trying to give the semaphore");
        if (xSemaphoreGive(display_mutex) == pdTRUE) {
          ESP_LOGI(TAG, "display mutex is given away first by task");
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1));
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

    if (uxQueueSpacesAvailable(person_queue) == 0) {
      ESP_LOGI(TAG, "Queue is full!");
    } else {
      ESP_LOGI(TAG, "Queue has space available: %d",
               uxQueueSpacesAvailable(person_queue));
    }

    // delay between adding persons
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
