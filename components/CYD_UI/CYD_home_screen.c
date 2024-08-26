#include "CYD_home_screen.h"

// TODO: make this static
lv_obj_t *create_home_screen() {
  lv_obj_t *screen = lv_obj_create(NULL);

  /* CREATE MAIN CONTENT */
  // TODO: break this down into pieces, not just image
  lv_obj_set_style_bg_color(screen, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);

  /* CREATE TOP BAR */
  create_top_bar(screen);
  
  /* CREATE BOTTOM MENU */
  create_bottom_menu(screen);
  return screen;
}
