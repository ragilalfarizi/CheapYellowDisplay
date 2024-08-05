#include "CYD_search_screen.h"

/* STATIC PROTOTYPE DECLARATION */
static void dot_pressed_cb(lv_event_t *e);

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
  lv_obj_t *radar = lv_image_create(screen);
  lv_image_set_src(radar, &icon_radar);
  lv_obj_set_pos(radar, 4, 31);

  /* CREATE DOTS */
  static lv_style_t style_dot;
  lv_style_init(&style_dot);
  lv_style_set_size(&style_dot, 15, 15);
  lv_style_set_radius(&style_dot, LV_RADIUS_CIRCLE);
  lv_style_set_bg_color(&style_dot, COLOR_TERTIARY);
  lv_style_set_border_width(&style_dot, 0);

  // TODO: use math to calculate and determine the position

  // first dot
  lv_obj_t *dot1 = lv_obj_create(radar);
  lv_obj_add_style(dot1, &style_dot, 0);
  lv_obj_align(dot1, LV_ALIGN_CENTER, 20, -50);
  lv_obj_add_event_cb(dot1, dot_pressed_cb, LV_EVENT_CLICKED,
                      (void *)"@michael_89 with 66 Bumps");

  // second dot
  lv_obj_t *dot2 = lv_obj_create(radar);
  lv_obj_add_style(dot2, &style_dot, 0);
  lv_obj_align(dot2, LV_ALIGN_CENTER, -70, 70);
  lv_obj_add_event_cb(dot2, dot_pressed_cb, LV_EVENT_CLICKED,
                      (void *)"@sara_lynn91 with 47 bumps");

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
