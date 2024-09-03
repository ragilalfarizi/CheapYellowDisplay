#include "CYD_UI.h"

#include "CYD_home_screen.h"
#include "CYD_search_screen.h"
#include "CYD_setting_screen.h"

/* STATIC PROTOTYPE DECLARATION */
static void menu_event_handler(lv_event_t *e);
static void dot_pressed_cb(lv_event_t *e);

/* GLOBAL VARIABLES DECLARATION */
lv_obj_t          *home_screen    = NULL;
lv_obj_t          *search_screen  = NULL;
lv_obj_t          *setting_screen = NULL;
extern screen_id_t current_screen;
static const char *TAG = "Main UI";

void setup_screens() {
  home_screen    = create_home_screen();
  search_screen  = create_search_screen();
  setting_screen = create_setting_screen();

  create_startup_screen();
  vTaskDelay(pdMS_TO_TICKS(3000));

  lv_screen_load(home_screen);
}

lv_obj_t *create_screen(lv_color_t color) {
  lv_obj_t *screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen, color, 0);

  create_bottom_menu(screen);
  return screen;
}

void create_startup_screen() {
  LV_IMAGE_DECLARE(boot_screen_img);

  lv_obj_t *boot_screen = lv_image_create(lv_screen_active());
  lv_image_set_src(boot_screen, &boot_screen_img);
  lv_obj_align(boot_screen, LV_ALIGN_CENTER, 0, 0);
}

static void menu_event_handler(lv_event_t *e) {
  screen_id_t id = (screen_id_t)lv_event_get_user_data(e);

  switch (id) {
    case HOME:
      lv_screen_load(home_screen);
      printf("switch to home screen\n");
      ESP_LOGI(TAG, "Switched to Home Screen");
      vTaskSuspend(add_rand_person_handler);
      break;

    case SEARCH:
      lv_screen_load(search_screen);
      ESP_LOGI(TAG, "Switched to Search Screen");
      // TODO: add flag that you're on search screen
      vTaskResume(add_rand_person_handler);
      break;

    case SETTINGS:
      lv_screen_load(setting_screen);
      ESP_LOGI(TAG, "Switched to Settings Screen");
      vTaskSuspend(add_rand_person_handler);
      break;

    default:
      break;
  }
}

void create_bottom_menu(lv_obj_t *parent) {
  // create style for box
  static lv_style_t style_bottom_box;
  lv_style_init(&style_bottom_box);
  lv_style_set_bg_color(&style_bottom_box, COLOR_ACCENT);
  lv_style_set_radius(&style_bottom_box, 0);
  lv_style_set_flex_flow(&style_bottom_box, LV_FLEX_FLOW_ROW);
  lv_style_set_flex_main_place(&style_bottom_box, LV_FLEX_ALIGN_SPACE_AROUND);
  lv_style_set_flex_cross_place(&style_bottom_box, LV_FLEX_ALIGN_CENTER);
  lv_style_set_layout(&style_bottom_box, LV_LAYOUT_FLEX);

  // create box container for the bottom menu
  lv_obj_t *bottom_box = lv_obj_create(parent);
  lv_obj_set_size(bottom_box, LV_PCT(100), 50);
  lv_obj_align(bottom_box, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_style(bottom_box, &style_bottom_box, 0);

  // create home button
  LV_IMAGE_DECLARE(home_icon);
  lv_obj_t *home_img = lv_imagebutton_create(bottom_box);
  lv_imagebutton_set_src(home_img, LV_IMAGEBUTTON_STATE_RELEASED, NULL,
                         &home_icon, NULL);
  lv_obj_add_event_cb(home_img, menu_event_handler, LV_EVENT_CLICKED,
                      (void *)HOME);

  // create search button
  LV_IMAGE_DECLARE(search_icon);
  lv_obj_t *search_img = lv_imagebutton_create(bottom_box);
  lv_imagebutton_set_src(search_img, LV_IMAGEBUTTON_STATE_RELEASED, NULL,
                         &search_icon, NULL);
  lv_obj_add_event_cb(search_img, menu_event_handler, LV_EVENT_CLICKED,
                      (void *)SEARCH);

  // create settings button
  LV_IMAGE_DECLARE(setting_icon);
  lv_obj_t *setting_img = lv_imagebutton_create(bottom_box);
  lv_imagebutton_set_src(setting_img, LV_IMAGEBUTTON_STATE_RELEASED, NULL,
                         &setting_icon, NULL);
  lv_obj_add_event_cb(setting_img, menu_event_handler, LV_EVENT_CLICKED,
                      (void *)SETTINGS);

  // FIX: the hitbox is in below and so small. resizing to a px doesn't fix the
  //      proglem. perhaps use .svg and set the size
  // HACK: there's a scrollbar because the size of the icon overlap the
  //      contianer
  // TODO: set 3 label, attach to logo
  // TODO: refactor assets name into img_xxx.c or ico_xxx.c or screen_xxx.c
}

void create_top_bar(lv_obj_t *screen) {
  // create style for top bar
  static lv_style_t style_top_bar;
  lv_style_init(&style_top_bar);
  lv_style_set_bg_color(&style_top_bar, COLOR_TERTIARY);
  lv_style_set_border_width(&style_top_bar, 0);
  lv_style_set_radius(&style_top_bar, 0);

  // create container for top bar
  lv_obj_t *container = lv_obj_create(screen);
  lv_obj_set_size(container, LV_PCT(100), 20);
  lv_obj_set_align(container, LV_ALIGN_TOP_MID);
  lv_obj_add_style(container, &style_top_bar, 0);

  // create style for label on top bar
  static lv_style_t style_top_label;
  lv_style_init(&style_top_label);
  lv_style_set_text_font(&style_top_label, LV_FONT_MONTSERRAT_10);
  lv_style_set_text_color(&style_top_label, COLOR_PRIMARY);
  lv_style_set_text_align(&style_top_label, LV_TEXT_ALIGN_CENTER);

  // TODO: change the font to Roboto Bold 10

  // create label for top bar
  lv_obj_t *label = lv_label_create(container);
  lv_obj_set_align(label, LV_ALIGN_CENTER);
  lv_obj_add_style(label, &style_top_label, 1);
  lv_label_set_text_static(label, "PRIVA RND INDONESIA");

  // create bell icon
  // FIX: the bell is squared. don't use .svg?
  LV_IMAGE_DECLARE(icon_bell);
  lv_obj_t *bell = lv_image_create(container);
  lv_image_set_src(bell, &icon_bell);
  lv_obj_set_align(bell, LV_ALIGN_RIGHT_MID);
  lv_obj_set_size(bell, 12, 12);
}
