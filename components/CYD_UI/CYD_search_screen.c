#include "CYD_search_screen.h"

/* STATIC PROTOTYPE DECLARATION */
static void dot_pressed_cb(lv_event_t *e);
static void create_dot(lv_obj_t *parent, Person_t *p);

/* VARIABLES DECLARATION */
static lv_obj_t     *radar;
static lv_style_t    style_dot;
static const char   *TAG = "Search Screen";
extern QueueHandle_t person_data_q;
extern Person_t      person;
extern lv_obj_t     *home_screen;

static void refresh_btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

  switch (code) {
      // Single tap: Take one data item from the queue
    case LV_EVENT_CLICKED:
      ESP_LOGI(TAG, "Button clicked, taking one data item");

      // Clear the radar first if the there are object
      if (radar != NULL) {
        clear_radar_display();
      }

      // TODO: fetch the amount of available data from queue and draw them
      if (xQueueReceive(person_data_q, &person, pdMS_TO_TICKS(1000)) ==
          pdPASS) {
        ESP_LOGI(TAG, "about to draw person");
        draw_dot_info(&person);
      } else {
        ESP_LOGI(TAG, "can't receive the data from queue");
      }
      break;

      // Long press: Start refreshing data at intervals
    case LV_EVENT_LONG_PRESSED:
      ESP_LOGI(TAG, "Button long pressed, starting data refresh");
      // TODO: Implement the instruction of refreshing data
      break;

      // Stop refreshing when the button is released
    case LV_EVENT_RELEASED:
      // NOTE: might be redundant.
      //       find another way to fetch the data while pressing the button.
      ESP_LOGI(TAG, "Button released, stopping data refresh");
      break;

    default:
      break;
  }
}

static void back_btn_event_cb(lv_event_t *e) {
  ESP_LOGI(TAG, "Back button is pressed");
  lv_screen_load(home_screen);
  vTaskSuspend(add_rand_person_handler);
}

// TODO: move into separate file
lv_obj_t *create_search_screen() {
  // create style for search screen
  static lv_style_t style_screen_search;
  lv_style_init(&style_screen_search);
  lv_style_set_bg_color(&style_screen_search, COLOR_PRIMARY);

  lv_obj_t *screen = lv_obj_create(NULL);
  lv_obj_add_style(screen, &style_screen_search, 0);

  /* MAIN CONTENT ON SEARCH SCREEN */

  // create radar
  LV_IMAGE_DECLARE(icon_radar);
  radar = lv_image_create(screen);
  lv_image_set_src(radar, &icon_radar);
  lv_obj_set_pos(radar, 4, 31);

  /* CREATE DOTS */
  lv_style_init(&style_dot);
  lv_style_set_size(&style_dot, 15, 15);
  lv_style_set_radius(&style_dot, LV_RADIUS_CIRCLE);
  lv_style_set_bg_color(&style_dot, COLOR_TERTIARY);
  lv_style_set_border_width(&style_dot, 0);

  // TODO: use math to calculate and determine the position

  /* CREATE TOP AND BOTTOM LAYER ON TOP OF MAIN CONTENT */
  create_top_bar(screen);
  // Create bottom button

  /* CREATE REFRESH BUTTON */
  lv_obj_t *refresh_btn = lv_button_create(screen);
  lv_obj_set_align(refresh_btn, LV_ALIGN_BOTTOM_MID);
  lv_obj_set_size(refresh_btn, 100, 50);
  lv_obj_add_event_cb(refresh_btn, refresh_btn_event_cb, LV_EVENT_ALL, NULL);

  lv_obj_t *label = lv_label_create(refresh_btn);  // Add a label to the button
  lv_label_set_text(label, "Refresh");
  lv_obj_center(label);

  /* CREATE BACK BUTTON */
  lv_obj_t *btn2 = lv_button_create(screen);
  lv_obj_set_align(btn2, LV_ALIGN_BOTTOM_LEFT);
  lv_obj_set_size(btn2, 40, 30);
  lv_obj_add_event_cb(btn2, back_btn_event_cb, LV_EVENT_PRESSED, NULL);

  lv_obj_t *label2 = lv_label_create(btn2);  // Add a label to the button
  lv_label_set_text(label2, "Back");
  lv_obj_center(label2);

  return screen;
}

static void dot_pressed_cb(lv_event_t *e) {
  ESP_LOGI(TAG, "dot is pressed");

  ESP_LOGI(TAG, "Person name is %s", person.name);

  // create the pop-up dialog
  // TODO: get the current screen with lv_event_t
  lv_obj_t *popup = lv_msgbox_create(lv_screen_active());
  lv_obj_center(popup);
  lv_msgbox_add_title(popup, "Info");
  lv_msgbox_add_text(popup, person.name);
  lv_msgbox_add_close_button(popup);
  // TODO: resize the message box
}

// Helper function to create a dot on the radar
static void create_dot(lv_obj_t *parent, Person_t *p) {
  if (parent == NULL) {
    ESP_LOGE(TAG, "Failed to create dot: parent object is NULL");
    return;
  }

  lv_obj_t *dot = lv_obj_create(parent);

  lv_obj_add_style(dot, &style_dot, 0);
  lv_obj_align(dot, LV_ALIGN_CENTER, p->pos_x, p->pos_y);
  lv_obj_add_event_cb(dot, dot_pressed_cb, LV_EVENT_CLICKED, (void *)p->name);

  ESP_LOGI(TAG, "Dot created for %s at X: %d, Y: %d", p->name, p->pos_x,
           p->pos_y);
}

// OPTIMIZE: remove this abstraction. just straight to create_dot
void draw_dot_info(Person_t *p) {
  create_dot(radar, p);
  vTaskDelay(pdMS_TO_TICKS(10));
}

void clear_radar_display() {
  if (radar == NULL) {
    ESP_LOGE(TAG, "Radar object is NULL");
    return;
  }

  ESP_LOGI(TAG, "Cleaning radar...");
  lv_obj_clean(radar);
  vTaskDelay(pdMS_TO_TICKS(100));
}
