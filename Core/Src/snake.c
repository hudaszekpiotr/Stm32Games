#include "snake.h"


Queue* createQueue(unsigned capacity)
{
    Queue* queue = (Queue*)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    queue->rear = capacity - 1;
    queue->array = (uint8_t*)malloc(
        queue->capacity * sizeof(uint8_t));
    return queue;
}

void enqueue(Queue* queue, uint8_t item){
	queue->rear = (queue->rear + 1) % queue->capacity;
	queue->array[queue->rear] = item;
	queue->size = queue->size + 1;
}

uint8_t deque(Queue* queue){
	uint8_t item = queue->array[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return item;
}

bool check_if_in_queue(Queue* queue, int elem) {
	for (int i = 0; i < queue->size; i++) {
		if (elem == queue->array[(queue->front + i)% queue->capacity]) {
			return true;
		}
	}
	return false;
}

uint8_t get_column(uint8_t i){
	return i/16;
}

uint8_t get_row(uint8_t i){
	return i%16;
}

void clear_space(uint16_t* frame, uint8_t enum_num){
	frame[get_column(enum_num)] &= ~(0b1 << get_row(enum_num));
}

void fill_space(uint16_t* frame, uint8_t enum_num){
	frame[get_column(enum_num)] |= 0b1 << get_row(enum_num);
}



uint8_t get_enum_num(uint8_t row, uint8_t col){
	return row*8 + col*8;
}

uint8_t get_new_apple_placement(Queue* queue){
	bool is_space_taken[256] = { false };
	for (int i = 0; i < queue->size; i++) {
		int elem = queue->array[(queue->front + i) % queue->capacity];
		is_space_taken[elem] = true;
	}

	uint8_t free_spaces[256] = { 0 };
	int free_spaces_arr_size = 0;
	for (int i = 0; i < 256; i++) {
		if (!is_space_taken[i]) {
			free_spaces[free_spaces_arr_size] = i;
			free_spaces_arr_size++;
		}
	}

	uint8_t random_place = rand() % free_spaces_arr_size;
	return free_spaces[random_place];

}


bool check_if_move_ok(uint16_t* frame, Queue* queue, int requested_pos) {
	if (requested_pos == queue->array[(queue->rear)-1]) {
		int last_pos = queue->array[queue->rear];
		requested_pos = last_pos + (last_pos - queue->array[queue->rear-1]);
		enqueue(queue, requested_pos);
		fill_space(frame, requested_pos);
		return true;
	}
	if (check_if_in_queue(queue, requested_pos) || requested_pos < 0 || requested_pos > 255) {
		return false;
	}

	int last_elem = queue->array[queue->rear];
	if (abs(get_row(last_elem)-get_row(requested_pos)) > 1) {
		return false;
	}
	enqueue(queue, requested_pos);
	fill_space(frame, requested_pos);
	return true;
}

bool is_apple_eaten(int apple_placement, Queue* queue) {
	return check_if_in_queue(queue, apple_placement);
}

uint16_t* snake_get_frame(Vector2D player_input, ControlMethod control_method, bool new_game){
	static uint16_t frame[16] = {	0b0000000100000000,
									0b0000000100000000,
									0b0000000100000000,
									0,0,0,0,0,0,0,0,0,0,0,0,0};

	// places on screen are enumerated in following way:
	// 15 14 13 12 11 10 9 8 7 6 5 4 3  2  1  0
	//                             ...  18 17 16
	static Queue* queue = 0;
	if(queue == 0){
		queue = createQueue(256);
	}
	static bool lost_game = false;
	static int lost_game_frame_counter = 0;
	static uint8_t apple_placement = 0;

	if (lost_game) {
		lost_game_frame_counter++;
		if (lost_game_frame_counter > LOST_GAME_FRAMES_SNAKE || new_game) {
			new_game = true;
			lost_game_frame_counter = 0;
		}else{
			return get_game_over_screen();
		}
	}

	if(new_game){
		lost_game = false;
		frame[0] = 0b0000000100000000;
		frame[1] = 0b0000000100000000;
		frame[2] = 0b0000000100000000;
		for(int i = 3; i < 16; i++){
			frame[i] = 0;
		}

	    queue->front = queue->size = 0;
	    queue->rear = queue->capacity - 1;
		enqueue(queue, 8);
		enqueue(queue, 24);
		enqueue(queue, 40);
		apple_placement = get_new_apple_placement(queue);
		fill_space(frame, apple_placement);
		return frame;
	}

	int last_pos = queue->array[queue->rear];
	int requested_position = 0;
	if(control_method == Dpad){
		if(player_input.x > 0){
			requested_position = last_pos - 1;
		}else if(player_input.x < 0){
			requested_position = last_pos + 1;
		}else if(player_input.y > 0){
			requested_position = last_pos - 16;
		}else if(player_input.y < 0){
			requested_position = last_pos + 16;
		}else{
			requested_position = last_pos + (last_pos - queue->array[queue->rear-1]);
		}
	}else{
		if(player_input.x > TILT_THRESHOLD && player_input.y > TILT_THRESHOLD){
			requested_position = last_pos - 17;
		}else if(player_input.x < -TILT_THRESHOLD && player_input.y < -TILT_THRESHOLD){
			requested_position = last_pos + 17;
		}else if(player_input.x > TILT_THRESHOLD && player_input.y < -TILT_THRESHOLD){
			requested_position = last_pos + 15;
		}else if(player_input.x < -TILT_THRESHOLD && player_input.y > TILT_THRESHOLD){
			requested_position = last_pos - 15;
		}else if(player_input.x > TILT_THRESHOLD){
			requested_position = last_pos - 1;
		}else if(player_input.x < -TILT_THRESHOLD){
			requested_position = last_pos + 1;
		}else if(player_input.y > TILT_THRESHOLD){
			requested_position = last_pos - 16;
		}else if(player_input.y < -TILT_THRESHOLD){
			requested_position = last_pos + 16;
		}else{
			requested_position = last_pos + (last_pos - queue->array[queue->rear-1]);
		}
	}
	if (!check_if_move_ok(frame, queue, requested_position)) {
		lost_game = true;
		return get_game_over_screen();
	}

	if(apple_placement == requested_position){
		apple_placement = get_new_apple_placement(queue);
		fill_space(frame, apple_placement);
	}else{
		uint8_t enum_num = deque(queue);
		clear_space(frame, enum_num);
	}

	return frame;
}
