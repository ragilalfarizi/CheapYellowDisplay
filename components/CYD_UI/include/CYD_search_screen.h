#ifndef CYD_SEARCH_SCREEN_H_
#define CYD_SEARCH_SCREEN_H_

#include "CYD_UI.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "person_handling.h"

#define MAX_PERSON_COUNT 4

// extern Person_t *person_array[MAX_PERSON_COUNT];

lv_obj_t *create_search_screen();

void update_radar(SemaphoreHandle_t display_mutex, size_t *person_count,
                  Person_t **person_array);

void refresh_radar(void);

void draw_dot_info(Person_t *p);

void clear_radar_display();

#endif  // !CYD_SEARCH_SCREEN_H_
