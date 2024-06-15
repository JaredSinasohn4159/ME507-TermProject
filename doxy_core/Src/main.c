/* USER CODE BEGIN Header */
/**!
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l4xx_hal.h"
#include "CLController.h"
#include "controller_driver.h"
#include "display_driver.h"
#include "encoder_handler.h"
#include "motor_driver.h"
#include "pitch_encoder_handler.h"
#include <stdio.h>
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/*!
 * @var con
 * The instance of the RC controller
 */
static RC_Controller con = {.timer = &htim3,
		  .timer = &htim3,
  	  	  .period1 = 1500,
		  .period2 = 1500,
		  .re1 = 0,
		  .re2 = 0,
		  .fe1 = 0,
		  .fe2 = 0,
		  .fe_flag1 = 0,
		  .fe_flag2 = 0,
	  	  	  };

/*!
 * @var m
 * The instance of the DC motor turning the strobe wheel
 */
static Motor m = { .timer = &htim1,
	       .channels = 1,
		   .duty_cycle = 0,
         .enable_flag = 1
  	  	  };

/*!
 * @var pitch
 * The encoder the pitch encoder uses to approximate inheriting the encoder class
 */
static Encoder pitch = {.timer = &htim5,
						.timing_timer = &htim6,
						.prev_count = 0,
						.curr_count = 0,
						.prev_time = 0,
						.curr_time = 1,
						.pos = 0,
						.speed = 0,
						.dx = 0,
						.dt = 0
};

/*!
 * @var mot_enc
 * The motor encoder instance
 */
static Encoder mot_enc = {.timer = &htim4,
						.timing_timer = &htim6,
						.prev_count = 0,
						.curr_count = 0,
						.prev_time = 0,
						.curr_time = 1,
						.pos = 0,
						.speed = 0,
						.dx = 0,
						.dt = 0
};

/*!
 * @var m_con
 * The closed-loop PIDF controller for speed control of the motor.  The current controller is just pf and kp and kf have been tuned accordingly.
 */
static CLController m_con = {
		.kp = 500,
		.ki = 0,
		.kd = 0,
		.kf = 706,
		.setpoint = 0,
		.eff = 0,
		.curr = 0,
		.err = 0,
		.err_acc = 0,
		.prev_err_index = 0,
		.initial_time = 0,
		.curr_time = 1,
		.slope = 0,
		.prev_err_list_length = 10,
		.prev_err_list = {0,0,0,0,0,0,0,0,0,0}
};

/*!
 * @var pe
 * The pitch encoder instance of the pitch selection knob
 */
static PitchEncoder pe = {.pitch = 0,
						  .encoder = &pitch,
						  .delta = 0
};

/*!
 * @var display
 * The instance of the i2c driven 16 segment display
 */

static Display display = {.hi2c = &hi2c2,
					  .curr_note = 0,
					  .huart = &huart2
};

/*!
 * @var t1state
 * The current state of the Display and note task
 */
uint8_t t1state = 0;

/*!
 * @var t2state
 * The current state of the motor task
 */
uint8_t t2state = 0;

//! These are buffers used for displaying stuff via uart during debugging.
uint8_t Buffer[50] = {0};
uint8_t Pos_Buffer[50] = {0};
uint8_t Speed_Buffer[50] = {0};
uint8_t Space[] = " - ";
uint8_t StartMSG[] = "Starting I2C Scanning: \r\n";
uint8_t EndMSG[] = "Done! \r\n\r\n";
uint8_t led_buff;
uint8_t Eff_Buffer[50] = {0};

/*!
 * @var motor_speeds
 * The list of speeds for the motor to be in tune for each note
 */
static int32_t motor_speeds[12] = {56320,59679,63222,66970,70963,75182,79647,84378,89395,94720,100352,106312};

/*!
 * @var ptch
 * The current pitch of the tuner
 */
static uint32_t ptch;

/*!
 * @var timmy
 * The time variable used to run the closed loop controller and read the motor encoder every 2ms
 */
static uint32_t timmy = 0;

/*!
 * @var eff
 * the effort to run the motor at in pwm%
 */
static int32_t eff = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
void display_task(uint8_t* state);
void motor_task (uint8_t* state);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*!
 * This function implements the Display and Note task finite state machine.
 * @param state The pointer to the current state of the task
 */
void display_task(uint8_t* state){
	switch(*state){
	case 0:
		pe.encoder->timer->Instance->CNT = ((htim5.Init.Period)+1)/2;
		*state = 1;
		break;
	case 1:
		ptch = get_pitch(&pe);
		display_note(&display,ptch);
		break;
	}
}

/*!
 * This function implements the Motor task finite state machine.
 * @param state The pointer to the current state of the task
 */
void motor_task (uint8_t* state){
	switch(*state){
		case 0:
			*state = 1;
			timmy = HAL_GetTick();
			break;
		case 1:
			if(HAL_GetTick() >= timmy + 2){
				timmy = HAL_GetTick();
				m_con.setpoint = motor_speeds[ptch];
				encoder_read_curr_state(&mot_enc);
				eff = run(&m_con,mot_enc.speed);
				motor_set_duty_cycle(&m, eff);
				break;
			}
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
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  //! initialize I2C
  MX_I2C2_Init();
  //! start the timers in their various modes.
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start_IT(&htim5, TIM_CHANNEL_ALL);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_Base_Start(&htim6);
  //! set the display driver enable pin and the motor driver enable pin
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2,GPIO_PIN_SET);
  //! This code was all used to debug the i2c driver to no avail, I have left it here for transparancy.
  /*uint8_t aTxBuffer[2] = {};
  aTxBuffer[0] = 0x00;
  aTxBuffer[1] = 0x00;
  uint8_t lvl = 85;
  uint8_t addr = 0b11000000;
  HAL_I2C_Mem_Read(&hi2c2, addr, 0x00, 1, led_buff, 1, 500);
    sprintf(Buffer, "Reg 0x14x: %X \r\n",led_buff);
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
  HAL_Delay(500);
  ret = HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x14, 1, lvl, 1, 500);
  /*HAL_Delay(500);
  if(ret!=HAL_OK){
	  sprintf(Buffer, "did not work :(\n\r");
	  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
  }else if(ret == HAL_OK)
  {
      sprintf(Buffer, "worked!\n\r");
      HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
  }
  HAL_Delay(500);
  ret = HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x15, 1, lvl, 1, 500);
    /*if(ret!=HAL_OK){
  	  sprintf(Buffer, "did not work :(\n\r");
  	  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }else if(ret == HAL_OK)
    {
        sprintf(Buffer, "worked!\n\r");
        HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }
  HAL_Delay(500);
  ret = HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x16, 1, lvl, 1, 500);
    /*if(ret!=HAL_OK){
  	  sprintf(Buffer, "did not work :(\n\r");
  	  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }else if(ret == HAL_OK)
    {
        sprintf(Buffer, "worked!\n\r");
        HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }
  HAL_Delay(500);
  ret = HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x17, 1, lvl, 1, 500);
    /*if(ret!=HAL_OK){
  	  sprintf(Buffer, "did not work :(\n\r");
  	  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }else if(ret == HAL_OK)
    {
        sprintf(Buffer, "worked!\n\r");
        HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }
  HAL_I2C_Mem_Read(&hi2c2, addr, 0x14, 1, (uint8_t*)led_buff, 1, 500);
  sprintf(Buffer, "Reg 0x14x: %X \r\n",led_buff);
  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
  HAL_I2C_Mem_Read(&hi2c2, addr, 0x15, 1, led_buff, 1, 500);
  sprintf(Buffer, "Reg 0x15x: %X \r\n",led_buff);
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
  HAL_I2C_Mem_Read(&hi2c2, addr, 0x16, 1, led_buff, 1, 500);
  sprintf(Buffer, "Reg 0x16x: %X \r\n",led_buff);
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
  HAL_I2C_Mem_Read(&hi2c2, addr, 0x17, 1, led_buff, 1, 500);
  sprintf(Buffer, "Reg 0x17x: %X \r\n",led_buff);
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
  /*if(ret!=HAL_OK){
  	  sprintf(Buffer, "did not work :(\n\r");
  	  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }else if(ret == HAL_OK)
    {
        sprintf(Buffer, "worked!\n\r");
        HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }*/
  //HAL_Delay(500);
  //ret = HAL_I2C_Master_Transmit(&hi2c2, 0b11000001, ((uint8_t*)1),1,100);
  /*if(ret!=HAL_OK){
  	  sprintf(Buffer, "did not work :(\n\r");
  	  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }else if(ret == HAL_OK)
    {
        sprintf(Buffer, "worked!\n\r");
        HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
    }
        uint32_t time1 = HAL_GetTick();
        uint32_t time2 = HAL_GetTick();
        int32_t sped = 0;
        //HAL_I2C_Master_Transmit((&hi2c2), 0b11000001, ((uint8_t*)0b1101100001111000),2,100);
        uint8_t prev_count = 1;

  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x02, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x03, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x04, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x05, 1, (uint8_t*)0xFF, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x06, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x07, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x08, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x09, 1, (uint8_t*)0xFF, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x0A, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x0B, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x0C, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x0D, 1, (uint8_t*)0xFF, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x0E, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x0F, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x10, 1, (uint8_t*)lvl, 1, 100);
  //HAL_I2C_Mem_Write(&hi2c2, 0b11000000, 0x11, 1, (uint8_t*)0xFF, 1, 100);
	// The folloqing code was taken from here: https://deepbluembedded.com/stm32-i2c-scanner-hal-code-example/
  uint8_t TLC59116_PWM0_AUTOINCR = 0x82;
  HAL_UART_Transmit(&huart2, StartMSG, sizeof(StartMSG), 10000);
      for(i=1; i<128; i++)
      {
          ret = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(i<<1), 3, 5);
          if (ret != HAL_OK)
          {
              HAL_UART_Transmit(&huart2, Space, sizeof(Space), 10000);
          }
          else if(ret == HAL_OK)
          {
              sprintf(Buffer, "0x%X", i);
              HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
          }
      }
      HAL_UART_Transmit(&huart2, EndMSG, sizeof(EndMSG), 10000);
  uint8_t bruh[] = { TLC59116_PWM0_AUTOINCR, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  while(HAL_I2C_Master_Transmit(&hi2c2, 0xC0, *bruh, sizeof(bruh), 100) != HAL_OK)
      {
      HAL_Delay(1);
	  if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
          {
              Error_Handler();
          }
      }*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //! @brief This is the game loop that runs forever to convert user inputed code into a PWM signal to drive the motors.
  while (1)
  {
	 //! run the tasks in round-robin style.
	display_task(&t1state);
	motor_task(&t2state);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0xF010F3FE;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 79;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 1073741823;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim5, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 79;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

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
  huart2.Init.BaudRate = 115200;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
