/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 *
 * @attention		L'HORLOGEUSE L476
 *
 * This is a simple clock, like an LCD watch, for STM32L476 Discovery kit (STM32L476G-DISCO).
 * Automatic standby mode when 30 s inactivity.
 * Navigation with joystick to display / set time and date.
 * Works with CR2032 battery.
 *
 * Copyright (c) 2025 Xavier Halgand & STMicroelectronics.
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
#include "i2c.h"
#include "lcd.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtc.h"
#include "application.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "sht4x_i2c.h"
#include "SHT45_appli.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_INACTIVITY_TIME		15000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

__IO uint32_t AppStatus = STATE_START;
__IO uint32_t inactivity_time = 0;
__IO uint32_t time_counter;
__IO uint32_t time_counter2;

FlagStatus JoyInitialized = RESET;
FlagStatus IddInitialized = RESET;
FlagStatus LcdInitialized = RESET;
FlagStatus LedInitialized = RESET;
JOYState_TypeDef JoyState = JOY_NONE;

FlagStatus ReturnFromStandbyShutdown = RESET;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_LCD_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	/*##--Configure minimum hardware resources at boot ########################*/
	SystemHardwareInit();

	BSP_LED_On(LED_RED);
	BSP_LED_On(LED_GREEN);
	HAL_Delay(500);
	BSP_LED_Off(LED_RED);
	BSP_LED_Off(LED_GREEN);

	SHT45_appli();

	/***********************************************************************************/

	/* Check if the system was resumed from StandBy mode */
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		/* Clear the Standby flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

		/* Check and Clear the Wakeup flag */
		if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF1) != RESET)
		{
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
		}

		/* Notify we return from Standby or Shutdown Low Power mode */
		ReturnFromStandbyShutdown = SET;
	}

	MX_RTC_Init();

	/*------------------------------------------------------------------------------*/
	if (ReturnFromStandbyShutdown == SET)
	{
		Display_WakeUp_msg();
	}
	else
	{
		Display_First_Start_msg();
	}

	time_counter = HAL_GetTick();
	time_counter2 = HAL_GetTick();
	inactivity_time = 0; // increased in systick interrupt handler (stm....it.c)
	AppStatus = STATE_DISPLAY_TIME;

	while (inactivity_time < MAX_INACTIVITY_TIME)
	{
		application();
	}

	/******************************* When MAX_INACTIVITY_TIME is reached, go to standby mode *********************/
	/******************************* Wake up with RESET or joystick center button **********/
	// 1) Clear The Wakeup Flag
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
	// 2) Enable The WKUP1 Pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	// 3) Enter The Standby Mode
	HAL_PWR_EnterSTANDBYMode();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	/**************************** Never reached **********************************/
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
	/******************************************************************************/
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/* USER CODE BEGIN 4 */
/***********************************************************************************************/
/**
 * @brief  System Power Configuration at Boot
 * @retval None
 */
void SystemHardwareInit(void)
{

	/* Init LED4 and LED5  */
	if (LedInitialized != SET)
	{
		BSP_LED_Init(LED_RED);
		BSP_LED_Init(LED_GREEN);
		LedInitialized = SET;
	}

	/* Init STM32L476G-Discovery joystick in interrupt mode */
//	if (JoyInitialized != SET) {
//		BSP_JOY_Init(JOY_MODE_EXTI);
//		JoyInitialized = SET;
//	}
	/* Initialize the LCD */
	if (LcdInitialized != SET)
	{
		BSP_LCD_GLASS_Init();
		LcdInitialized = SET;
	}
	/* Enable PWR clock */
	__HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * @brief  System Low Power Configuration
 * @retval None
 */
void SystemHardwareDeInit(void)
{
	RCC_OscInitTypeDef oscinitstruct =
	{ 0 };

	if (LedInitialized != RESET)
	{
		BSP_LED_DeInit(LED_RED);
		BSP_LED_DeInit(LED_GREEN);
		LedInitialized = RESET;
	}

	if (JoyInitialized != RESET)
	{
		BSP_JOY_DeInit();
		JoyInitialized = RESET;
	}

	if (LcdInitialized != RESET)
	{
		BSP_LCD_GLASS_DeInit();
		LcdInitialized = RESET;
	}

	/* LSE off in all modes */
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) != RESET)
	{
		oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
		oscinitstruct.PLL.PLLState = RCC_PLL_NONE;
		oscinitstruct.LSEState = RCC_LSE_OFF;
		if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK)
		{
			while (1)
				;
		}
	}

	HAL_PWR_DisableBkUpAccess();

	/* Disable remaining clocks */
	__HAL_RCC_PWR_CLK_DISABLE();
	HAL_PWREx_DisablePullUpPullDownConfig();

	/* Configure MFX I2C Rx/Tx as Analog floating during IDD */
	//HAL_GPIO_DeInit(DISCOVERY_I2C2_SCL_GPIO_PORT, (DISCOVERY_I2C2_SCL_PIN | DISCOVERY_I2C2_SDA_PIN)); /* No STOP2 after FW upgrade */
	__HAL_RCC_FLASH_CLK_DISABLE();
	__HAL_RCC_SYSCFG_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_I2C2_CLK_DISABLE();

	RCC->AHB1SMENR = 0x0;
	RCC->AHB2SMENR = 0x0;
	RCC->AHB3SMENR = 0x0;
	RCC->APB1SMENR1 = 0x0;
	RCC->APB1SMENR2 = 0x0;
	RCC->APB2SMENR = 0x0;
}

/**
 * @brief EXTI line detection callbacks.
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	application_JOY_callback(GPIO_Pin);
}

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
