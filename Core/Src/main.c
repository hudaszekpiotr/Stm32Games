/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
bool menu_button_edge_falling = false;
bool up_button_edge_falling = false;
bool down_button_edge_falling = false;
bool left_button_edge_falling = false;
bool right_button_edge_falling = false;
Vector2D player_input_dpad = {0, 0};
Vector2D player_input_accelerometr = {0, 0};

bool in_menu = true;
bool new_game = true;
MenuScreen menu_screen = Snake;
GameName game_name = Snake;
ControlMethod control_method = Dpad;
int difficulty = 1;
int screen_brightness = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void set_screens_register(uint8_t addr, uint8_t data){
	HAL_GPIO_WritePin(GPIOA, CS_Pin, GPIO_PIN_RESET);
	for(int i = 0; i < 4; i++){
		HAL_SPI_Transmit(&hspi1, &addr, 1, HAL_MAX_DELAY);
		HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
	}
	HAL_GPIO_WritePin(GPIOA, CS_Pin, GPIO_PIN_SET);
}

void setup_screen(){
	uint8_t REG_DECODE_MODE = 0x0B;
	uint8_t REG_INTENSITY = 0x0A;
	uint8_t REG_SCAN_LIMIT = 0x0B;
	uint8_t REG_SHUTDOWN = 0x0C;
	uint8_t REG_DISP_TEST  = 0x0F;

	set_screens_register(REG_SHUTDOWN, 0x01);
	set_screens_register(REG_SCAN_LIMIT, 0x07);
	set_screens_register(REG_INTENSITY, 0x00);
	set_screens_register(REG_DECODE_MODE, 0xFF);

	for(int i = 1; i <= 8; i++){
		set_screens_register(i, 0x00);
	}

	set_screens_register(REG_DISP_TEST, 0x01);
	HAL_Delay(100);
	set_screens_register(REG_DISP_TEST, 0x00);
}

void draw_frame(uint16_t * array) {
	uint8_t* array_8_t = ((uint8_t *)array);

	for(int i = 0; i < 8; i++){
		HAL_GPIO_WritePin(GPIOA, CS_Pin, GPIO_PIN_RESET);
		uint8_t num = i + 1;

		HAL_SPI_Transmit(&hspi1, &num, 1, HAL_MAX_DELAY);
		HAL_SPI_Transmit(&hspi1, &array_8_t[2*i+1], 1, HAL_MAX_DELAY);

		HAL_SPI_Transmit(&hspi1, &num, 1, HAL_MAX_DELAY);
		HAL_SPI_Transmit(&hspi1, &array_8_t[2*i], 1, HAL_MAX_DELAY);

		HAL_SPI_Transmit(&hspi1, &num, 1, HAL_MAX_DELAY);
		HAL_SPI_Transmit(&hspi1, &array_8_t[16 + 2*i+1], 1, HAL_MAX_DELAY);

		HAL_SPI_Transmit(&hspi1, &num, 1, HAL_MAX_DELAY);
		HAL_SPI_Transmit(&hspi1, &array_8_t[16 + 2*i], 1, HAL_MAX_DELAY);

		HAL_GPIO_WritePin(GPIOA, CS_Pin, GPIO_PIN_SET);
	}

}

void setup_accelerometr() {
	uint8_t val_bw_rate = 0b00000000;//100Hz
	uint8_t val_power_ctl = 0b00001000;//turn on
	uint8_t val_data_format = 0b00000000;// +-2g

	HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, REG_POWER_CTL, 1, &val_power_ctl, sizeof(val_power_ctl), HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, REG_DATA_FORMAT, 1, &val_data_format, sizeof(val_data_format), HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, ADXL345_ADDR, REG_BW_RATE, 1, &val_bw_rate, sizeof(val_bw_rate), HAL_MAX_DELAY);
}

Vector2D get_accelerometr_input(){
	int16_t x_val;
	int16_t y_val;
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_ADDR, DATAX0 | 0x80, 1, (uint8_t*)&x_val, sizeof(x_val), HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_ADDR, DATAY0 | 0x80, 1, (uint8_t*)&y_val, sizeof(y_val), HAL_MAX_DELAY);

	Vector2D vector = {(float)x_val/0x7FFF, (float)y_val/0x7FFF};
	return vector;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin){
	case MENU_BUTTON_Pin:
		menu_button_edge_falling = true;
		break;
	case UP_BUTTON_Pin:
		up_button_edge_falling = true;
		break;
	case DOWN_BUTTON_Pin:
		down_button_edge_falling = true;
		break;
	case LEFT_BUTTON_Pin:
		left_button_edge_falling = true;
		break;
	case RIGHT_BUTTON_Pin:
		right_button_edge_falling = true;
		break;
	}
}

bool debounce_button(GPIO_TypeDef *port, uint16_t pin, bool *edge_var, int* counter){
	if(*edge_var){
		if(HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET){
			*edge_var = false;
			return false;
		}
		if(*counter > 5){
			*counter = 0;
			*edge_var = false;
			return true;
		}
		(*counter)++;
	}
	return false;
}

void check_buttons(){
	static int menu_button_stable_counter = 0;
	static int up_button_stable_counter = 0;
	static int down_button_stable_counter = 0;
	static int left_button_stable_counter = 0;
	static int right_button_stable_counter = 0;

	if(debounce_button(GPIOB, MENU_BUTTON_Pin, &menu_button_edge_falling, &menu_button_stable_counter)){
		in_menu = true;
		switch(menu_screen){
		case SnakeMenu:
			draw_frame(get_snake_menu_screen());
			break;
		case Brightness:
			draw_frame(get_brightness_menu_screen(screen_brightness));
			break;
		case Difficulty:
			draw_frame(get_difficulty_menu_screen(difficulty));
			break;
		case ControlMethodMenu:
			draw_frame(get_control_method_menu_screen(control_method));
			break;
		}
	}
	if(debounce_button(GPIOB, UP_BUTTON_Pin, &up_button_edge_falling, &up_button_stable_counter)){
		player_input_dpad.x = 0;
		player_input_dpad.y = 1;
		if(in_menu){
			switch(menu_screen){
			case SnakeMenu:
				in_menu = false;
				game_name = Snake;
				new_game = true;
				player_input_dpad.x = 0;
				player_input_dpad.y = 0;
				break;
			case Brightness:
				if(screen_brightness < 16) screen_brightness++;
				draw_frame(get_brightness_menu_screen(screen_brightness));
				break;
			case Difficulty:
				if(difficulty < 16) difficulty++;
				draw_frame(get_difficulty_menu_screen(difficulty));
				break;
			case ControlMethodMenu:
				control_method = Dpad;
				draw_frame(get_control_method_menu_screen(control_method));
				break;
			}
		}
	}
	if(debounce_button(GPIOA, DOWN_BUTTON_Pin, &down_button_edge_falling, &down_button_stable_counter)){
		player_input_dpad.x = 0;
		player_input_dpad.y = -1;
		if(in_menu){
			switch(menu_screen){
			case SnakeMenu:
				in_menu = false;
				game_name = Snake;
				new_game = true;
				player_input_dpad.x = 0;
				player_input_dpad.y = 0;
				break;
			case Brightness:
				if(screen_brightness > 1) screen_brightness--;
				draw_frame(get_brightness_menu_screen(screen_brightness));
				break;
			case Difficulty:
				if(difficulty > 1) difficulty--;
				draw_frame(get_difficulty_menu_screen(difficulty));
				break;
			case ControlMethodMenu:
				control_method = Accelerometer;
				draw_frame(get_control_method_menu_screen(control_method));
				break;
			}
		}
	}
	if(debounce_button(GPIOC, LEFT_BUTTON_Pin, &left_button_edge_falling, &left_button_stable_counter)){
		player_input_dpad.x = -1;
		player_input_dpad.y = 0;
		if(in_menu){
			switch(menu_screen){
			case SnakeMenu:
				menu_screen = Brightness;
				draw_frame(get_brightness_menu_screen(screen_brightness));
				break;
			case ControlMethodMenu:
				menu_screen = Snake;
				draw_frame(get_snake_menu_screen());
				break;
			case Difficulty:
				menu_screen = ControlMethodMenu;
				draw_frame(get_control_method_menu_screen(control_method));
				break;
			case Brightness:
				menu_screen = Difficulty;
				draw_frame(get_difficulty_menu_screen(difficulty));
				break;
			}
		}
	}
	if(debounce_button(GPIOA, RIGHT_BUTTON_Pin, &right_button_edge_falling, &right_button_stable_counter)){
		player_input_dpad.x = 1;
		player_input_dpad.y = 0;
		if(in_menu){
			switch(menu_screen){
			case SnakeMenu:
				menu_screen = ControlMethodMenu;
				draw_frame(get_control_method_menu_screen(control_method));
				break;
			case ControlMethodMenu:
				menu_screen = Difficulty;
				draw_frame(get_difficulty_menu_screen(difficulty));
				break;
			case Difficulty:
				menu_screen = Brightness;
				draw_frame(get_brightness_menu_screen(screen_brightness));
				break;
			case Brightness:
				menu_screen = Snake;
				draw_frame(get_snake_menu_screen());
				break;
			}
		}
	}
}

void play_game_timer_it(){
	Vector2D player_input;
	if(control_method == Dpad){
		player_input = player_input_dpad;
	}else{
		player_input = get_accelerometr_input();
	}

	uint16_t* frame;
	switch(game_name){
	case Snake:
		frame = snake_get_frame(player_input, control_method, new_game);
		new_game = false;
		break;
	case Tetris:
		break;
	}
	draw_frame(frame);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim16 && !in_menu)
  {
	  play_game_timer_it();
  }
  if (htim == &htim17 ){
	  check_buttons();
  }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  setup_screen();
  setup_accelerometr();
  HAL_TIM_Base_Start_IT(&htim16);
  HAL_TIM_Base_Start_IT(&htim17);
  draw_frame(get_snake_menu_screen());



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_Delay (1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_TIM16|RCC_PERIPHCLK_TIM17;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.Tim16ClockSelection = RCC_TIM16CLK_HCLK;
  PeriphClkInit.Tim17ClockSelection = RCC_TIM17CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 71;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 10000-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 71;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 10000;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LEFT_BUTTON_Pin */
  GPIO_InitStruct.Pin = LEFT_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(LEFT_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RIGHT_BUTTON_Pin DOWN_BUTTON_Pin */
  GPIO_InitStruct.Pin = RIGHT_BUTTON_Pin|DOWN_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MENU_BUTTON_Pin UP_BUTTON_Pin */
  GPIO_InitStruct.Pin = MENU_BUTTON_Pin|UP_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
