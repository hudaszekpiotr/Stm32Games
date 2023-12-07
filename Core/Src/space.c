#include <space.h>


bool spawn_obstacles(uint16_t* slow_frame, uint16_t* medium_frame, uint16_t* fast_frame){
	unsigned obstacle_speed;
	unsigned x_position;

	unsigned random_num = rand()%1000;
	if(random_num > 100){
		return false;
	}
	if(random_num > 80 ){
		obstacle_speed = 0;
	}else if(random_num > 40 ){
		obstacle_speed = 1;
	}else{
		obstacle_speed = 2;
	}

	switch (obstacle_speed) {
	case 0:
		uint16_t obstacle_shape1[5] = {0b0000000000000100,
										0b0000000000001110,
										0b0000000000011111,
										0b0000000000001110,
										0b0000000000000100 };
		x_position = rand() % 11;
		for (int i = 0; i < 5; i++) {
			obstacle_shape1[i] = obstacle_shape1[i] << x_position;
			slow_frame[i] |= obstacle_shape1[i];
		}
		break;
	case 1:
		uint16_t obstacle_shape2[3] = {0b0000000000000010,
										0b0000000000000111,
										0b0000000000000010 };
		x_position = rand() % 13;
		for (int i = 0; i < 3; i++) {
			obstacle_shape2[i] = obstacle_shape2[i] << x_position;
			medium_frame[i] |= obstacle_shape2[i];
		}
		break;
	case 2:
		uint16_t obstacle_shape3[1] = { 0b0000000000000001};
		x_position = rand() % 16;
		for (int i = 0; i < 1; i++) {
			obstacle_shape3[i] = obstacle_shape3[i] << x_position;
			fast_frame[i] |= obstacle_shape3[i];
		}
		break;
	}
	return true;
}

void move_obstacles(uint16_t* slow_frame, uint16_t* medium_frame, uint16_t* fast_frame, bool restart) {
	static unsigned clock_div = 0;
	if (restart) {
		clock_div = 0;
	}

	if (clock_div % 3 == 0) {
		for (int i = 19; i > 0; i--) {
			slow_frame[i] = slow_frame[i - 1];
		}
		slow_frame[0] = 0;
	}

	if (clock_div % 2 == 0) {
		for (int i = 17; i > 0; i--) {
			medium_frame[i] = medium_frame[i - 1];
		}
		medium_frame[0] = 0;
	}

	for (int i = 15; i > 0; i--) {
		fast_frame[i] = fast_frame[i - 1];
	}
	fast_frame[0] = 0;

	clock_div++;
	if (clock_div >= 6) {
		clock_div = 0;
	}
}

void combine_frames(uint16_t* frame, uint16_t* slow_frame, uint16_t* medium_frame, uint16_t* fast_frame) {
	for (int i = 0; i < 16; i++) {
		frame[i] = slow_frame[i+4] | medium_frame[i+2] | fast_frame[i];
	}
}

bool move_player(uint16_t* frame, Vector2D player_input, ControlMethod control_method, bool restart) {
	static uint16_t player_shape[3] = {	0b0000000100000000,
										0b0000000100000000,
										0b0000001110000000 };
	static unsigned y_position = 13;

	if (restart) {
		player_shape[0] = 0b0000000100000000;
		player_shape[1] = 0b0000000100000000;
		player_shape[2] = 0b0000001110000000;
		y_position = 13;
	}

	if (player_input.x > TILT_THRESHOLD_RACER) {
		bool move_ok = true;
		for (int i = 0; i < 3; i++) {
			if (player_shape[i] & 1) {
				move_ok = false;
			}
		}
		for (int i = 0; i < 3 && move_ok; i++) {
			player_shape[i] = player_shape[i] >> 1;
		}
	}
	else if (player_input.x < -TILT_THRESHOLD_RACER) {
		bool move_ok = true;
		for (int i = 0; i < 3; i++) {
			if (player_shape[i] & 0b1000000000000000) {
				move_ok = false;
			}
		}
		for (int i = 0; i < 3 && move_ok; i++) {
			player_shape[i] = player_shape[i] << 1;
		}
	}
	else if (player_input.y < -TILT_THRESHOLD_RACER && y_position < 13) {
		y_position++;
	}
	else if (player_input.y > TILT_THRESHOLD_RACER && y_position > 0) {
		y_position--;
	}

	for (int i = y_position; i < y_position + 3; i++) {
		if (frame[i] & player_shape[i-y_position]) {
			return false;
		}
		frame[i] |= player_shape[i-y_position];
	}
	return true;

}

uint16_t* space_get_frame(Vector2D player_input, ControlMethod control_method, bool new_game) {
	static uint16_t frame[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static uint16_t slow_frame[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	static uint16_t medium_frame[18] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	static uint16_t fast_frame[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	for(int i = 0; i < 15; i++){
		frame[i] = 0;
	}

	static bool lost_game = false;
	static int lost_game_frame_counter = 0;
	static bool restart = true;

	if (lost_game) {
		lost_game_frame_counter++;
		if (lost_game_frame_counter > LOST_GAME_FRAMES_RACER || new_game) {
			new_game = true;
			lost_game_frame_counter = 0;
		}else{
			return get_game_over_screen();
		}
	}

	if (new_game) {
		for (int i = 0; i < 20; i++) {
			if (i > 17) {
				slow_frame[i] = 0;
				continue;
			}
			if (i > 15) {
				slow_frame[i] = 0;
				medium_frame[i] = 0;
				continue;
			}
			frame[i] = 0;
			slow_frame[i] = 0;
			medium_frame[i] = 0;
			fast_frame[i] = 0;
		}
		lost_game = false;
		restart = true;
		return frame;
	}

	spawn_obstacles(slow_frame, medium_frame, fast_frame);

	move_obstacles(slow_frame, medium_frame, fast_frame, restart);
	combine_frames(frame, slow_frame, medium_frame, fast_frame);

	if (!move_player(frame, player_input, control_method, restart)) {
		lost_game = true;
	}
	restart = false;

	return frame;
}
