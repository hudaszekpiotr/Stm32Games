#include "tetris.h"

bool move_possible(uint16_t* frame, Block* block) {
	for (int i = 0; i < 4; i++) {
		if((block->row) + i == 16 && block->shape[i] != 0){
			return false;
		}
		if ((block->row) + i < 16 && (frame[(block->row) + i] & (block->shape[i]))) {
			return false;
		}
	}
	return true;
}

bool spawn_new_block(uint16_t* frame, Block* block) {
	uint8_t block_num = rand() % 7;
	switch (block_num) {
	case 0:
		block->shape[0] = 0b0000001111000000;
		block->shape[1] = 0;
		block->type = 'I';
		break;
	case 1:
		block->shape[0] = 0b0000001000000000;
		block->shape[1] = 0b0000001110000000;
		block->type = 'J';
		break;
	case 2:
		block->shape[0] = 0b0000000010000000;
		block->shape[1] = 0b0000001110000000;
		block->type = 'L';
		break;
	case 3:
		block->shape[0] = 0b0000001100000000;
		block->shape[1] = 0b0000001100000000;
		block->type = 'O';
		break;
	case 4:
		block->shape[0] = 0b0000000110000000;
		block->shape[1] = 0b0000001100000000;
		block->type = 'S';
		break;
	case 5:
		block->shape[0] = 0b0000000100000000;
		block->shape[1] = 0b0000001110000000;
		block->type = 'T';
		break;
	case 6:
		block->shape[0] = 0b0000001100000000;
		block->shape[1] = 0b0000000110000000;
		block->type = 'Z';
		break;
	}
	block->shape[2] = 0;
	block->shape[3] = 0;
	block->row = 0;
	block->rotation_center = 8;

	if (move_possible(frame, block)) {
		draw_block(frame, block);
		return true;
	}
	return false;
}

void rotate_block(uint16_t* frame, Block* block) {
	clear_prev_block_pos(frame, block);
	Block* new_block = malloc(sizeof(*new_block));
	new_block->row = block->row;
	new_block->type = block->type;
	new_block->rotation_center = block->rotation_center;
	memcpy(new_block->shape, block->shape, sizeof(uint16_t)*4);
	bool did_wall_kick = false;

	if(block->type == 'I'){
		int rot_center = new_block->rotation_center;
		if(!get_bit(new_block->shape[2], rot_center) && !get_bit(new_block->shape[1], rot_center-1)){
			if(new_block->row != 0){
				new_block->row--;
			}
			new_block->shape[1] = new_block->shape[0];
			new_block->shape[0] = 0;
		}

		if(rot_center == 16){
			new_block->shape[0] = 0;
			new_block->shape[1] = 0;
			new_block->shape[2] = 0b1111<<12;
			new_block->shape[3] = 0;
			new_block->rotation_center = 14;
		}else if(rot_center == 0){
			new_block->shape[0] = 0;
			new_block->shape[1] = 0b1111<<0;
			new_block->shape[2] = 0;
			new_block->shape[3] = 0;
			new_block->rotation_center = 2;
		}else if(get_bit(new_block->shape[1], rot_center) && get_bit(new_block->shape[1], rot_center-1)){
			unsigned val = 0b0010<<(rot_center-2);									//0000
			new_block->shape[0] = val;												//1111
			new_block->shape[1] = val; 												//0000
			new_block->shape[2] = val;												//0000
			new_block->shape[3] = val;
		}else if(get_bit(new_block->shape[1], rot_center-1) && get_bit(new_block->shape[2], rot_center-1)){
			if(rot_center == 1){													//0010
				new_block->rotation_center = 2;										//0010
			}
			new_block->shape[0] = 0;
			new_block->shape[1] = 0;
			new_block->shape[2] = 0b1111<<(new_block->rotation_center-2);
			new_block->shape[3] = 0;

		}else if(get_bit(new_block->shape[2], rot_center-1) && get_bit(new_block->shape[2], rot_center)){	//0000
			unsigned val = 0b0100<<(rot_center-2);										//0000
			new_block->shape[0] = val;													//1111
			new_block->shape[1] = val; 													//0000
			new_block->shape[2] = val;
			new_block->shape[3] = val;
		}else{																	//0100
			if(rot_center == 15){							//0100
				new_block->rotation_center = 14;								//0100
			}																	//0100
			new_block->shape[0] = 0;
			new_block->shape[1] = 0b1111<<(new_block->rotation_center-2);
			new_block->shape[2] = 0;
			new_block->shape[3] = 0;
		}
	}else if(block->type != 'O'){
		if(new_block->rotation_center == 0){
			for (int i = 0; i < 4; i++) {
				new_block->shape[i] = block->shape[i] << 1;
			}
			did_wall_kick = true;
			new_block->rotation_center = 1;
		}else if(new_block->rotation_center == 15){
			for (int i = 0; i < 4; i++) {
				new_block->shape[i] = block->shape[i] >> 1;
			}
			did_wall_kick = true;
			new_block->rotation_center = 14;
		}
		rotate(new_block->shape, new_block->rotation_center, 1);
	}

	if (!move_possible(frame, new_block) && !did_wall_kick) {		//wall kicks
		if(!move_block_right(frame, new_block)){
			move_block_left(frame, new_block);
		}
	}
	if(move_possible(frame, new_block)){
		memcpy(block->shape, new_block->shape, sizeof(uint16_t)*4);
		block-> row = new_block->row;
		block -> rotation_center = new_block->rotation_center;
	}
	free(new_block);
	draw_block(frame, block);
}

bool get_bit(uint16_t num, int pos){
	return ((num) & (1<<(pos)));
}

void write_bit(uint16_t* num, int pos, bool val){
	if(val){
		*num |= (1<<pos);
	}else{
	*num &= ~(1<<pos);
	}
}

void rotate_90_clockwise(uint16_t* arr, int x, int y)
{
	for (int j = x+1; j > x-1; j--) {
		int temp = get_bit(arr[0], j);
		write_bit(&arr[0], j, get_bit(arr[1 + j - x], 0));
		write_bit(&arr[3 + j - x], 0, get_bit(arr[2], 3 + j - x));
		write_bit(&arr[2], (3 + j - x), get_bit(arr[-j + x - 1], 2));
		write_bit(&arr[-j+x-1], 3-1, temp);
	}
}

void rotate(uint16_t* arr, int x, int y) {
	bool temp = get_bit(arr[y-1], x-1);
	write_bit(&arr[y-1], x-1, get_bit(arr[y-1], x+1));
	bool temp2 = get_bit(arr[y+1], x-1);
	write_bit(&arr[y+1], x-1, temp);
	temp = get_bit(arr[y+1], x+1);
	write_bit(&arr[y+1], x+1, temp2);
	write_bit(&arr[y-1], x+1, temp);

	temp = get_bit(arr[y], x-1);
	write_bit(&arr[y], x-1, get_bit(arr[y-1], x));
	temp2 = get_bit(arr[y+1], x);
	write_bit(&arr[y+1], x, temp);
	temp = get_bit(arr[y], x+1);
	write_bit(&arr[y], x+1, temp2);
	write_bit(&arr[y-1], x, temp);
	;
}

void clear_prev_block_pos(uint16_t* frame, Block* block) {
	for (int i = 0; i < 4 && ((block->row) + i < 16); i++) {
		uint16_t var = ~(block->shape[i]);
		frame[(block->row) + i] &= var;
	}
}

void draw_block(uint16_t* frame, Block* block) {
	for (int i = 0; i < 4 && ((block->row) + i < 16); i++) {
		frame[(block->row) + i] |= block->shape[i];
	}
}


bool move_block_down(uint16_t* frame, Block* block) {
	clear_prev_block_pos(frame, block);
	Block* new_block = malloc(sizeof(*new_block));
	new_block->row = (block->row) + 1;
	new_block->type = block->type;
	new_block->rotation_center = block->rotation_center;
	memcpy(new_block->shape, block->shape, sizeof(uint16_t)*4);

	if (move_possible(frame, new_block)) {
		draw_block(frame, new_block);
		block->row = new_block->row;
		free(new_block);
		return true;
	}
	draw_block(frame, block);
	free(new_block);
	return false;
}

bool move_block_right(uint16_t* frame, Block* block) {
	clear_prev_block_pos(frame, block);
	Block* new_block = malloc(sizeof(*new_block));
	new_block->row = block->row;
	new_block->type = block->type;
	new_block->rotation_center = block->rotation_center - 1;
	int num;
	for (int i = 0; i < 4; i++) {
		num = block->shape[i];
		if (num & 0x1) {
			draw_block(frame, block);
			free(new_block);
			return false;
		}
		new_block->shape[i] = block->shape[i] >> 1;
	}

	if (move_possible(frame, new_block)) {
		draw_block(frame, new_block);
		block->rotation_center = new_block->rotation_center;
		memcpy(block->shape, new_block->shape, sizeof(uint16_t)*4);
		free(new_block);
		return true;
	}
	draw_block(frame, block);
	free(new_block);
	return false;
}

bool move_block_left(uint16_t* frame, Block* block) {
	clear_prev_block_pos(frame, block);
	Block* new_block = malloc(sizeof(*new_block));
	new_block->row = block->row;
	new_block->type = block->type;
	new_block->rotation_center = block->rotation_center + 1;
	int num;
	for (int i = 0; i < 4; i++) {
		num = block->shape[i];
		if (num & 0x8000) {
			draw_block(frame, block);
			free(new_block);
			return false;
		}
		new_block->shape[i] = block->shape[i] << 1;
	}

	if (move_possible(frame, new_block)) {
		draw_block(frame, new_block);
		block->rotation_center = new_block->rotation_center;
		memcpy(block->shape, new_block->shape, sizeof(uint16_t)*4);
		free(new_block);
		return true;
	}
	draw_block(frame, block);
	free(new_block);
	return false;
}

void clear_bottom(uint16_t* frame) {
	for (int i = 15; i >= 0; i--) {
		if (frame[i] == 0b1111111111111111) {
			for (int j = i - 1; j >= 0; j--) {
				frame[j + 1] = frame[j];
			}
			frame[0] = 0;
			i++;
		}
	}
}



uint16_t* tetris_get_frame(Vector2D player_input_dpad, Vector2D player_input_accelerometer, ControlMethod control_method, bool new_game) {
	static uint16_t frame[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static Block block;
	static int frame_counter = 0;

	static bool lost_game = false;
	static int lost_game_frame_counter = 0;
	Vector2D player_input;
	switch(control_method){
	case Dpad:
		player_input = player_input_dpad;
		break;
	case Accelerometer:
		player_input = player_input_accelerometer;
		break;
	}

	if (lost_game) {
		lost_game_frame_counter++;
		if (lost_game_frame_counter > LOST_GAME_FRAMES_TETRIS || new_game) {
			new_game = true;
			lost_game_frame_counter = 0;
		}else{
			return get_game_over_screen();
		}
	}

	if (new_game) {
		for (int i = 0; i < 16; i++) {
			frame[i] = 0;
		}
		spawn_new_block(frame, &block);
		lost_game = false;
		return frame;
	}




	if (player_input.x > TILT_THRESHOLD_TETRIS) {
		move_block_right(frame, &block);
	}
	else if (player_input.x < -TILT_THRESHOLD_TETRIS) {
		move_block_left(frame, &block);
	}
	else if (player_input_dpad.y > 0) {
		rotate_block(frame, &block);
	}
	else if (player_input.y < -TILT_THRESHOLD_TETRIS) {
		if (!move_block_down(frame, &block)) {
			if(!spawn_new_block(frame, &block)){
				lost_game = true;
			}
			clear_bottom(frame);
			return frame;
		}
	}
	if(frame_counter > 4){
		frame_counter = 0;
		if (!move_block_down(frame, &block)) {
			if(!spawn_new_block(frame, &block)){
				lost_game = true;
			}
		}
	}
	frame_counter++;

	clear_bottom(frame);
	return frame;
}
