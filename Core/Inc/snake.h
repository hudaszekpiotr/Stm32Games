#ifndef SRC_SNAKE_H_
#define SRC_SNAKE_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "menu_screens.h"
#include "enums_types.h"

#define TILT_THRESHOLD 0.2
#define LOST_GAME_FRAMES_SNAKE 8

#include "snake.h"


typedef struct Queue {
    int front, rear, size;
    unsigned capacity;
    uint8_t* array;
} Queue;

Queue* createQueue(unsigned capacity);

void enqueue(Queue* queue, uint8_t item);

uint8_t deque(Queue* queue);

bool check_if_in_queue(Queue* queue, int elem);

uint8_t get_column(uint8_t i);

uint8_t get_row(uint8_t i);

void clear_space(uint16_t* frame, uint8_t enum_num);

void fill_space(uint16_t* frame, uint8_t enum_num);

uint8_t get_enum_num(uint8_t row, uint8_t col);

uint8_t get_new_apple_placement(Queue* queue);

bool check_if_move_ok(uint16_t* frame, Queue* queue, int requested_pos);

bool is_apple_eaten(int apple_placement, Queue* queue);

uint16_t* snake_get_frame(Vector2D player_input, ControlMethod control_method, bool new_game);


#endif
