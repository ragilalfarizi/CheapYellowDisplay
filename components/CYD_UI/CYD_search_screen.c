#include "CYD_search_screen.h"

/* STATIC PROTOTYPE DECLARATION */
static void        dot_pressed_cb(lv_event_t *e);
static void        create_dot(lv_obj_t *parent, Person_t *p);
static lv_obj_t   *radar;
static lv_style_t  style_dot;
static const char *TAG = "Search Screen";

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
  // static lv_style_t style_dot;
  lv_style_init(&style_dot);
  lv_style_set_size(&style_dot, 15, 15);
  lv_style_set_radius(&style_dot, LV_RADIUS_CIRCLE);
  lv_style_set_bg_color(&style_dot, COLOR_TERTIARY);
  lv_style_set_border_width(&style_dot, 0);

  // TODO: use math to calculate and determine the position

  // first dot
  // lv_obj_t *dot1 = lv_obj_create(radar);
  // lv_obj_add_style(dot1, &style_dot, 0);
  // lv_obj_align(dot1, LV_ALIGN_CENTER, 20, -50);
  // lv_obj_add_event_cb(dot1, dot_pressed_cb, LV_EVENT_CLICKED,
  //                     (void *)"@michael_89 with 66 Bumps");

  // second dot
  // lv_obj_t *dot2 = lv_obj_create(radar);
  // lv_obj_add_style(dot2, &style_dot, 0);
  // lv_obj_align(dot2, LV_ALIGN_CENTER, -70, 70);
  // lv_obj_add_event_cb(dot2, dot_pressed_cb, LV_EVENT_CLICKED,
  //                     (void *)"@sara_lynn91 with 47 bumps");

  // FIX: the mcu reset everytime i close sara lynn pop up

  /* CREATE TOP AND BOTTOM LAYER ON TOP OF MAIN CONTENT */
  create_top_bar(screen);
  create_bottom_menu(screen);
  return screen;
}

static void dot_pressed_cb(lv_event_t *e) {
  printf("dot ditekan\n");

  const char *info = (const char *)lv_event_get_user_data(e);

  // create the pop-up dialog
  // TODO: get the current screen with lv_event_t
  lv_obj_t *popup = lv_msgbox_create(lv_screen_active());
  lv_obj_center(popup);
  lv_msgbox_add_title(popup, "Info");
  lv_msgbox_add_text(popup, info);
  lv_msgbox_add_close_button(popup);
  // TODO: resize the message box
}

// Helper function to create a dot on the radar
static void create_dot(lv_obj_t *parent, Person_t *p) {
  printf("mau gambar dot\n");
  ESP_LOGI(TAG, "About to draw a dot");
  if (parent == NULL) {
    ESP_LOGE(TAG, "failed to create dot\n");
    return;
  }

  lv_obj_t *dot = lv_obj_create(parent);

  lv_obj_add_style(dot, &style_dot, 0);
  lv_obj_align(dot, LV_ALIGN_CENTER, p->pos_x, p->pos_y);
  lv_obj_add_event_cb(dot, dot_pressed_cb, LV_EVENT_CLICKED, (void *)p->name);
}

void draw_dot_info(Person_t *p) {
  ESP_LOGI(TAG, "Entering draw_dot_info()");
  for (uint8_t i = 0; i < 2; i++) {
    printf("Name: %s, X: %d, Y: %d\n", p[i].name, p[i].pos_x, p[i].pos_y);
    create_dot(radar, &p[i]);
  }
  ESP_LOGI(TAG, "The dots have been placed");
}

void clear_radar_display() {
  if (radar == NULL) {
    ESP_LOGE(TAG, "Radar object is NULL");
    return;
  }

  ESP_LOGI(TAG, "Entering obj clean");
  lv_obj_clean(radar);
  vTaskDelay(pdMS_TO_TICKS(1000));
}
