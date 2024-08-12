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

/* TASK HANDLER DECLARATION */
TaskHandle_t lvgl_display_task_handler      = NULL;
TaskHandle_t i2c_data_handling_task_handler = NULL;
TaskHandle_t radar_display_task_handler     = NULL;

// TODO: create dynamic allocation for Person Struct using dynamic array

/* GLOBAL VARIABLES */
static const char   *TAG            = "main";
size_t               person_count   = 0;
volatile screen_id_t current_screen = HOME;
uint8_t              capacity       = INITIAL_PERSON_CAPACITY;
uint8_t              current_size   = 0;

// this is allocating data dynamically. WIP.
// temporarily using stack allocation.
// Person_t            *person_array[MAX_PERSON_COUNT];
// Person_t p_arr[MAX_PERSON_COUNT];
Person_t *persons;

// Person_t p_arr[0] = p1;

QueueHandle_t     serialized_json_data_queue;
SemaphoreHandle_t display_mutex;

void app_main(void) {
  // Person_t *persons;
  // uint8_t   capacity = INITIAL_PERSON_CAPACITY;
  // uint8_t   size     = 0;

  allocate_person_dynamically(&persons, &capacity);

  Person_t p1 = {111, "ragil", 90, 80};
  Person_t p2 = {222, "ahmad", -20, -70};

  add_person(&persons, &p1, &current_size);
  add_person(&persons, &p2, &current_size);

  // TODO: change the size into the lenght of upcoming serialized json data
  serialized_json_data_queue = xQueueCreate(10, sizeof(Person_t));
  display_mutex              = xSemaphoreCreateMutex();

  /* LCD HW initialization */
  ESP_LOGI(TAG, "Entering LCD_INIT");
  LCD_init();

  /* LCD HW initialization */
  // SDCard_init();

  /* Touch initialization */
  ESP_LOGI(TAG, "Entering touch_init");
  touch_init();

  // i2c_init();
  // allocate_person_dynamically();

  /* LVGL initialization */
  ESP_LOGI(TAG, "Entering app_lvgl_init");
  app_lvgl_init();

  setup_screens();
  // assign_dummy_data();

  /* TASK CREATION */
  xTaskCreatePinnedToCore(lvgl_display_task, "LVGL Display Task", 4096,
                          NULL, 5, &lvgl_display_task_handler, 0);
  // xTaskCreatePinnedToCore(I2C_data_handling_task, "I2C Data Handling", 4096,
  //                         NULL, 3, &i2c_data_handling_task_handler, 1);
  xTaskCreatePinnedToCore(radar_display_task, "Radar Display Task", 3584,
                          NULL, 3, &radar_display_task_handler, 1);

  vTaskSuspend(radar_display_task_handler);
}

void lvgl_display_task(void *pvParameter) {
  lv_init();

  while (1) {
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void I2C_data_handling_task(void *pvParameter) {
  // setup i2c

  // while (1)
  //    fetch_data
  //    send to queue

  // ============= //
  /*
   while True:
        json_data = receive_data_from_i2c()
        if json_data:
            push_to_queue(i2c_queue, json_data)
        delay(periodic_interval)
  */
}

void radar_display_task(void *pvParameter) {
  // task is now active, start processing
  while (1) {
    // update_dot_info(p_arr, new_p); // using queue? mutex?
    printf("masuk task radar\n");

    // TODO: fetch from queue and deserialize

    draw_dot_info(persons);

    // wait 3 seconds
    vTaskDelay(pdMS_TO_TICKS(3000));

    clear_radar_display();

    // // check if we should supsend (notification)
    // if (ulTaskNotifyTake(pdTRUE, 0)) {
    //   break;
    // }
  }
}
//
// void assign_dummy_data() {
//   // Check if the array can hold more persons
//   if (person_count < MAX_PERSON_COUNT) {
//     // Allocate memory for each Person_t object
//     for (size_t i = 0; i < MAX_PERSON_COUNT; i++) {
//       person_array[i] = (Person_t *)malloc(sizeof(Person_t));
//       if (person_array[i] != NULL) {
//         person_array[i]->id = i + 1;
//         snprintf(person_array[i]->name, sizeof(person_array[i]->name),
//                  "Person %zu", i + 1);
//         person_array[i]->pos_x = 10 + (i * 20);  // Example positions
//         person_array[i]->pos_y = 30 + (i * 20);
//         person_count++;
//       } else {
//         printf("Memory allocation failed for person %zu\n", i + 1);
//       }
//     }
//   } else {
//     printf("Person array is already full.\n");
//   }
// }
//
// void print_person_data() {
//   // Print the person data for testing
//   for (size_t i = 0; i < person_count; i++) {
//     printf("Person ID: %d\n", person_array[i]->id);
//     printf("Person Name: %s\n", person_array[i]->name);
//     printf("Position X: %d, Position Y: %d\n", person_array[i]->pos_x,
//            person_array[i]->pos_y);
//     printf("-------------------\n");
//   }
// }
