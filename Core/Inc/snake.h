#ifndef SRC_SNAKE_H_
#define SRC_SNAKE_H_

#include <stdint.h>
#include <stdbool.h>
#include "enums_types.h"

uint16_t* snake_get_frame(Vector2D player_input, ControlMethod control_method, bool new_game);

#endif
