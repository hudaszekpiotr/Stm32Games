#include "menu_screens.h"

uint16_t* get_snake_menu_screen(){
	static uint16_t snake_menu_screen[16] = {	0b0000000000000000,
												0b0111101001001100,
												0b0100001101010010,
												0b0111101011011110,
												0b0000101001010010,
												0b0111101001010010,
												0b0000000000000000,
												0b0001001001111000,
												0b0001010001000000,
												0b0001100001111000,
												0b0001010001000000,
												0b0001001001111000,
												0b0000000000000000,
												0b0111011101110110,
												0b0101110111011100,
												0b0000000000000000};
	return snake_menu_screen;
}

uint16_t* get_tetris_menu_screen(){
	static uint16_t tetris_menu_screen[16] = {	0b0000000000000000,
												0b0111101111011110,
												0b0010001000001000,
												0b0010001111001000,
												0b0010001000001000,
												0b0010001111001000,
												0b0000000000000000,
												0b0111100100011110,
												0b0100100100010000,
												0b0111100100011110,
												0b0101000100000010,
												0b0100100100011110,
												0b0000000000000000,
												0b0010001101000110,
												0b0111011001110110,
												0b0000000000000000};
	return tetris_menu_screen;
}

uint16_t* get_difficulty_menu_screen(int difficulty){
	static uint16_t difficulty_menu_screen[16]={0b0000000000000000,
												0b0100001111010010,
												0b0100001000010010,
												0b0100001111010010,
												0b0100001000001100,
												0b0111101111001100,
												0b0000000000000000,
												0b0001111001000000,
												0b0001000001000000,
												0b0001111001000000,
												0b0001000001000000,
												0b0001111001111000,
												0b0000000000000000,
												0b0000000000000000,
												0b0000000000000000,
												0b0000000000000000};


	difficulty_menu_screen[13] = 65536 - pow(2, 16-difficulty);
	difficulty_menu_screen[14] = 65536 - pow(2, 16-difficulty);
	return difficulty_menu_screen;
}

uint16_t* get_brightness_menu_screen(int brightness){
	static uint16_t brightness_menu_screen[16]={0b0000000000000000,
												0b0111100111011110,
												0b0100001000010010,
												0b0111101000011110,
												0b0000101000010100,
												0b0111100111010010,
												0b0000000000000000,
												0b0111101111010010,
												0b0100001000011010,
												0b0111101111010110,
												0b0100001000010010,
												0b0111101111010010,
												0b0000000000000000,
												0b0000000000000000,
												0b0000000000000000,
												0b0000000000000000};

	brightness_menu_screen[13] = 65536 - pow(2, 16-brightness);
	brightness_menu_screen[14] = 65536 - pow(2, 16-brightness);
	return brightness_menu_screen;
}

uint16_t* get_control_method_menu_screen(ControlMethod control_method){
	static uint16_t control_method_menu_screen[16]={0b0000000000000000,
													0b0000010001001000,
													0b0000010001101000,
													0b0000010001011000,
													0b0000010001001000,
													0b0000010001001000,
													0b0000000000000000,
													0b0111101001011110,
													0b0100101001001000,
													0b0111101001001000,
													0b0100001001001000,
													0b0100001111001000,
													0b0000000000000000,
													0b0000000000000000,
													0b0000000000000000,
													0b0000000000000000};

	if(control_method == Dpad){
		control_method_menu_screen[13] = 0b0000000110000000;
		control_method_menu_screen[14] = 0b0000001111000000;
		control_method_menu_screen[15] = 0b0000000110000000;
	}else{
		control_method_menu_screen[13] = 0b0000001111000000;
		control_method_menu_screen[14] = 0b0000001111000000;
		control_method_menu_screen[15] = 0b0000001111000000;
	}
	return control_method_menu_screen;
}

