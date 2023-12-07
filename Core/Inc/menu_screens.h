#ifndef SRC_MENU_SCREENS_H_
#define SRC_MENU_SCREENS_H_

#include <stdint.h>
#include <stdbool.h>
#include "enums_types.h"
#include <math.h>

uint16_t* get_game_over_screen();

uint16_t* get_snake_menu_screen();

uint16_t* get_tetris_menu_screen();

uint16_t* get_space_menu_screen();

uint16_t* get_difficulty_menu_screen(int difficulty);

uint16_t* get_brightness_menu_screen(int brightness);

uint16_t* get_control_method_menu_screen(ControlMethod control_method);


#endif
