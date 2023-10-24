
#ifndef INC_TETRIS_H_
#define INC_TETRIS_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "enums_types.h"
#include "menu_screens.h"
#include <string.h>
#define LOST_GAME_FRAMES_TETRIS 30
#define TILT_THRESHOLD_TETRIS 0.2


#include "tetris.h"

typedef struct Block {
	int row;
	int rotation_center;
	char type;
	uint16_t shape[4];
} Block;

bool move_possible(uint16_t* frame, Block* block);

bool spawn_new_block(uint16_t* frame, Block* block);

void rotate_block(uint16_t* frame, Block* block);

bool get_bit(uint16_t num, int pos);

void write_bit(uint16_t* num, int pos, bool val);

void rotate(uint16_t* arr, int x, int y);

void clear_prev_block_pos(uint16_t* frame, Block* block);

void draw_block(uint16_t* frame, Block* block);

bool move_block_down(uint16_t* frame, Block* block);

bool move_block_right(uint16_t* frame, Block* block);

bool move_block_left(uint16_t* frame, Block* block);

void clear_bottom(uint16_t* frame);

uint16_t* tetris_get_frame(Vector2D player_input, Vector2D player_input_accelerometer, ControlMethod control_method, bool new_game);



#endif /* INC_TETRIS_H_ */
