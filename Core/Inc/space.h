/*
 * space.h
 *
 *  Created on: Nov 7, 2023
 *      Author: hudaszek
 */

#ifndef INC_SPACE_H_
#define INC_SPACE_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "enums_types.h"
#include "menu_screens.h"
#include <string.h>
#define LOST_GAME_FRAMES_RACER 30
#define TILT_THRESHOLD_RACER 0.1


bool spawn_obstacles(uint16_t* slow_frame, uint16_t* medium_frame, uint16_t* fast_frame);

void move_obstacles(uint16_t* slow_frame, uint16_t* medium_frame, uint16_t* fast_frame, bool restart);

void combine_frames(uint16_t* frame, uint16_t* slow_frame, uint16_t* medium_frame, uint16_t* fast_frame);

bool move_player(uint16_t* frame, Vector2D player_input, ControlMethod control_method, bool restart);

uint16_t* space_get_frame(Vector2D player_input, ControlMethod control_method, bool new_game);

#endif /* INC_SPACE_H_ */
