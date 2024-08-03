#include "CYD_UI.h"

// static const char *TAG = "UI MANAGER";

lv_obj_t *home_screen;
lv_obj_t *search_screen;
lv_obj_t *setting_screen;

LV_IMAGE_DECLARE(home_icon);
LV_IMAGE_DECLARE(search_icon);
LV_IMAGE_DECLARE(setting_icon);

// TODO: Seperate them into own files
// lv_color_t color1 = lv_color_hex(0x222831);
// lv_color_t color2 = lv_color_hex(0x31363F);
// lv_color_t color3 = lv_color_hex(0x76ABAE);
// lv_color_t color4 = lv_color_hex(0xEEEEEE);

static void menu_event_handler(lv_event_t *e) {
  printf("menu_event_handler is called\n");

  screen_id_t     id   = (screen_id_t)lv_event_get_user_data(e);

  switch (id) {
    case HOME:
      lv_screen_load(home_screen);
      printf("switch to home screen\n");
      break;

    case SEARCH:
      lv_screen_load(search_screen);
      printf("switch to search screen\n");
      break;

    case SETTINGS:
      lv_screen_load(setting_screen);
      printf("switch to setting screen\n");
      break;

    default:
      break;
  }
}

static void create_bottom_menu(lv_obj_t *parent) {
  // create style for box
  static lv_style_t style_bottom_box;
  lv_style_init(&style_bottom_box);
  lv_style_set_bg_color(
      &style_bottom_box,
      lv_color_hex(0xEEEEEE));  // TODO: refactor the color to be readable
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
                      (void*)HOME);

  // create search button
  LV_IMAGE_DECLARE(search_icon);
  lv_obj_t *search_img = lv_imagebutton_create(bottom_box);
  lv_imagebutton_set_src(search_img, LV_IMAGEBUTTON_STATE_RELEASED, NULL,
                         &search_icon, NULL);
  lv_obj_add_event_cb(search_img, menu_event_handler, LV_EVENT_CLICKED,
                      (void*)SEARCH);

  // create settings button
  LV_IMAGE_DECLARE(setting_icon);
  lv_obj_t *setting_img = lv_imagebutton_create(bottom_box);
  lv_imagebutton_set_src(setting_img, LV_IMAGEBUTTON_STATE_RELEASED, NULL,
                         &setting_icon, NULL);
  lv_obj_add_event_cb(setting_img, menu_event_handler, LV_EVENT_CLICKED,
                      (void*)SETTINGS);

  // FIX: the hitbox is in below and so small. resizing to a px doesn't fix the
  //      proglem. perhaps use .svg and set the size
  // HACK: there's a scrollbar because the size of the icon overlap the
  //      contianer
  // TODO: set 3 label, attach to logo
  // TODO: refactor assets name into img_xxx.c or ico_xxx.c or screen_xxx.c
}

void create_startup_screen() {
  LV_IMAGE_DECLARE(boot_screen_img);

  lv_obj_t *boot_screen = lv_image_create(lv_screen_active());
  lv_image_set_src(boot_screen, &boot_screen_img);
  lv_obj_align(boot_screen, LV_ALIGN_CENTER, 0, 0);
}

lv_obj_t *create_screen(lv_color_t color) {
  lv_obj_t *screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen, color, 0);

  create_bottom_menu(screen);
  return screen;
}

// TODO: treat this as entry point so it doesn't have global variable
void setup_screens() {
  // TODO: refactor the creation of screens so i don't have to have color bg

  // TODO: declare images and icon
  home_screen    = create_screen(lv_color_hex(0xFF0000));
  search_screen  = create_screen(lv_color_hex(0x00FF00));
  setting_screen = create_screen(lv_color_hex(0x0000FF));

  // create_bottom_menu(home_screen);
  // create_bottom_menu(search_screen);
  // create_bottom_menu(setting_screen);

  // TODO: add startup screen to this

  lv_screen_load(home_screen);
}
