/*
 * enums_types.h
 *
 *  Created on: Oct 2, 2023
 *      Author: hudaszek
 */

#ifndef INC_ENUMS_TYPES_H_
#define INC_ENUMS_TYPES_H_

typedef struct Vector2D {
	float x;
	float y;
} Vector2D;

typedef struct ScreenPosition {
	unsigned x;
	unsigned y;
} ScreenPosition;

typedef enum  {
	Snake,
	Tetris
} GameName;

typedef enum  {
	Dpad,
	Accelerometer
} ControlMethod;

typedef enum  {
	SnakeMenu,
	ControlMethodMenu,
	Difficulty,
	Brightness
} MenuScreen;


#endif /* INC_ENUMS_TYPES_H_ */
