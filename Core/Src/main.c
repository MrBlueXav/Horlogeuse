/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention	Simple clock, like an LCD watch, for STM32L476 Discovery kit (STM32L476G-DISCO).
 * Automatic standby mode when 30 s inactivity.
 * Navigation with joystick to see / set time and date.
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
#include "lcd.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtc.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	STATE_START, /*  */
	STATE_DISPLAY_TIME, /*  */
	STATE_DISPLAY_DATE, /*  */
	STATE_DISPLAY_DAY, /*  */
	STATE_GET_DAY, /*  */
	STATE_SET_DAY, /*  */
	STATE_SAVE_DAY, /*  */
	STATE_GET_TIME, /* */
	STATE_SET_HOURS, /*  */
	STATE_SET_MINS, /*  */
	STATE_SET_SECS, /*  */
	STATE_SAVE_TIME, /*  */
	STATE_GET_DATE, /*  */
	STATE_SET_DAY_NBR, /*  */
	STATE_SET_MONTH, /*  */
	STATE_SET_YEAR, /*  */
	STATE_SAVE_DATE, /*  */

} Appli_StateTypeDef;

typedef enum
{
	MSBCDIGIT, LSBCDIGIT

} BCDigit_TypeDef;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_INACTIVITY_TIME		30000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern __IO uint8_t bLCDGlass_KeyPressed;

const char *weekDay[] = { "LUNDI", "MARDI", "MERCRE", "JEUDI", "VENDRE", "SAMEDI", "DIMAN" }; // in french
uint8_t weekDayNbr;
RTC_TimeTypeDef theTime;
RTC_TimeTypeDef binaryTime;
RTC_DateTypeDef theDate;
RTC_DateTypeDef binaryDate;

uint32_t AppStatus = STATE_START;

__IO uint32_t inactivity_time = 0;

FlagStatus JoyInitialized = RESET;
FlagStatus IddInitialized = RESET;
FlagStatus LcdInitialized = RESET;
FlagStatus LedInitialized = RESET;
JOYState_TypeDef JoyState = JOY_NONE;

FlagStatus ReturnFromStandbyShutdown = RESET;
__IO bool displayMustBeUpdated = true;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

static void Display_First_Start_msg(void);
static void Display_WakeUp_msg(void);
static uint8_t convert_BCD_to_ASCII(uint8_t bcd_data, BCDigit_TypeDef digit);

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
  /* USER CODE BEGIN 2 */

	/*##--Configure minimum hardware resources at boot ########################*/
	SystemHardwareInit();

	BSP_LED_On(LED4);
	BSP_LED_On(LED5);
	HAL_Delay(500);
	BSP_LED_Off(LED4);
	BSP_LED_Off(LED5);

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

		/* read backup register 0 to check is we return from Idd measurement test */
		//IddIndex = SystemRtcBackupRead();
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

	uint32_t time_counter = HAL_GetTick();
	uint32_t time_counter2 = HAL_GetTick();
	inactivity_time = 0; // increased in systick interrupt handler (stm....it.c)

	uint8_t bufSec[2];
	uint8_t bufMin[2];
	uint8_t bufHour[2];
	uint8_t bufDate[2];
	uint8_t bufMonth[2];
	uint8_t bufYear[2];

	AppStatus = STATE_DISPLAY_TIME;

	while (inactivity_time < MAX_INACTIVITY_TIME)
	{

		if (HAL_GetTick() - time_counter2 > 50)
		{
			time_counter2 = HAL_GetTick();
			displayMustBeUpdated = true;
		}

		switch (AppStatus)
		{
		/*-------------------------------------------------------------------------------------*/
		case STATE_DISPLAY_TIME:

			if (HAL_GetTick() - time_counter > 1000)
			{
				time_counter = HAL_GetTick();

				HAL_RTC_GetTime(&hrtc, &theTime, RTC_FORMAT_BCD);
				HAL_RTC_GetDate(&hrtc, &theDate, RTC_FORMAT_BCD);

				bufHour[0] = convert_BCD_to_ASCII(theTime.Hours, MSBCDIGIT);
				bufHour[1] = convert_BCD_to_ASCII(theTime.Hours, LSBCDIGIT);
				bufMin[0] = convert_BCD_to_ASCII(theTime.Minutes, MSBCDIGIT);
				bufMin[1] = convert_BCD_to_ASCII(theTime.Minutes, LSBCDIGIT);
				bufSec[0] = convert_BCD_to_ASCII(theTime.Seconds, MSBCDIGIT);
				bufSec[1] = convert_BCD_to_ASCII(theTime.Seconds, LSBCDIGIT);

				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufHour[0], POINT_OFF, DOUBLEPOINT_OFF, 0);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufHour[1], POINT_OFF, DOUBLEPOINT_ON, 1);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMin[0], POINT_OFF, DOUBLEPOINT_OFF, 2);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMin[1], POINT_OFF, DOUBLEPOINT_ON, 3);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufSec[0], POINT_OFF, DOUBLEPOINT_OFF, 4);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufSec[1], POINT_OFF, DOUBLEPOINT_OFF, 5);
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_DISPLAY_DATE:

			if (HAL_GetTick() - time_counter > 1000)
			{
				time_counter = HAL_GetTick();

				HAL_RTC_GetTime(&hrtc, &theTime, RTC_FORMAT_BCD);
				HAL_RTC_GetDate(&hrtc, &theDate, RTC_FORMAT_BCD);

				bufDate[0] = convert_BCD_to_ASCII(theDate.Date, MSBCDIGIT);
				bufDate[1] = convert_BCD_to_ASCII(theDate.Date, LSBCDIGIT);
				bufMonth[0] = convert_BCD_to_ASCII(theDate.Month, MSBCDIGIT);
				bufMonth[1] = convert_BCD_to_ASCII(theDate.Month, LSBCDIGIT);
				bufYear[0] = convert_BCD_to_ASCII(theDate.Year, MSBCDIGIT);
				bufYear[1] = convert_BCD_to_ASCII(theDate.Year, LSBCDIGIT);

				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufDate[0], POINT_OFF, DOUBLEPOINT_OFF, 0);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufDate[1], POINT_ON, DOUBLEPOINT_OFF, 1);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMonth[0], POINT_OFF, DOUBLEPOINT_OFF, 2);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMonth[1], POINT_ON, DOUBLEPOINT_OFF, 3);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufYear[0], POINT_OFF, DOUBLEPOINT_OFF, 4);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufYear[1], POINT_OFF, DOUBLEPOINT_OFF, 5);
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_DISPLAY_DAY:

			if (HAL_GetTick() - time_counter > 1000)
			{
				time_counter = HAL_GetTick();
				HAL_RTC_GetTime(&hrtc, &theTime, RTC_FORMAT_BCD);
				HAL_RTC_GetDate(&hrtc, &theDate, RTC_FORMAT_BCD);
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayString((uint8_t*) weekDay[theDate.WeekDay - 1]); /* WeekDay is in [1...7] ! */
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_GET_TIME:

			//BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_ALLSEG_ALLCOM, LCD_BLINKFREQUENCY_DIV512);

			HAL_RTC_GetTime(&hrtc, &theTime, RTC_FORMAT_BCD);
			HAL_RTC_GetDate(&hrtc, &theDate, RTC_FORMAT_BCD);
			binaryTime.Hours = RTC_Bcd2ToByte(theTime.Hours);
			binaryTime.Minutes = RTC_Bcd2ToByte(theTime.Minutes);
			binaryTime.Seconds = RTC_Bcd2ToByte(theTime.Seconds);
			AppStatus = STATE_SET_HOURS;
			displayMustBeUpdated = true;
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_HOURS:

			if (displayMustBeUpdated)
			{
				bufHour[0] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryTime.Hours), MSBCDIGIT);
				bufHour[1] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryTime.Hours), LSBCDIGIT);
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufHour[0], POINT_OFF, DOUBLEPOINT_OFF, 0);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufHour[1], POINT_OFF, DOUBLEPOINT_ON, 1);
				displayMustBeUpdated = false;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_MINS:

			if (displayMustBeUpdated)
			{
				bufMin[0] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryTime.Minutes), MSBCDIGIT);
				bufMin[1] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryTime.Minutes), LSBCDIGIT);
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMin[0], POINT_OFF, DOUBLEPOINT_OFF, 2);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMin[1], POINT_OFF, DOUBLEPOINT_ON, 3);
				displayMustBeUpdated = false;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_SECS:

			if (displayMustBeUpdated)
			{
				bufSec[0] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryTime.Seconds), MSBCDIGIT);
				bufSec[1] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryTime.Seconds), LSBCDIGIT);
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufSec[0], POINT_OFF, DOUBLEPOINT_OFF, 4);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufSec[1], POINT_OFF, DOUBLEPOINT_OFF, 5);
				displayMustBeUpdated = false;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SAVE_TIME:

			theTime.Hours = RTC_ByteToBcd2(binaryTime.Hours);
			theTime.Minutes = RTC_ByteToBcd2(binaryTime.Minutes);
			theTime.Seconds = RTC_ByteToBcd2(binaryTime.Seconds);
			HAL_RTC_SetTime(&hrtc, &theTime, RTC_FORMAT_BCD);
			BSP_LCD_GLASS_Clear();
			BSP_LCD_GLASS_DisplayString((uint8_t*) "SAVED");
			HAL_Delay(2000);
			BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
			AppStatus = STATE_DISPLAY_TIME;
			break;

			/*-------------------------------------------------------------------------------------*/

		case STATE_GET_DATE:

			//BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_ALLSEG_ALLCOM, LCD_BLINKFREQUENCY_DIV512);

			HAL_RTC_GetTime(&hrtc, &theTime, RTC_FORMAT_BCD);
			HAL_RTC_GetDate(&hrtc, &theDate, RTC_FORMAT_BCD);
			binaryDate.Date = RTC_Bcd2ToByte(theDate.Date);
			binaryDate.Month = RTC_Bcd2ToByte(theDate.Month);
			binaryDate.Year = RTC_Bcd2ToByte(theDate.Year);
			AppStatus = STATE_SET_DAY_NBR;
			displayMustBeUpdated = true;
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_DAY_NBR:

			if (displayMustBeUpdated)
			{
				bufDate[0] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryDate.Date), MSBCDIGIT);
				bufDate[1] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryDate.Date), LSBCDIGIT);
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufDate[0], POINT_OFF, DOUBLEPOINT_OFF, 0);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufDate[1], POINT_ON, DOUBLEPOINT_OFF, 1);
				displayMustBeUpdated = false;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_MONTH:

			if (displayMustBeUpdated)
			{
				bufMonth[0] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryDate.Month), MSBCDIGIT);
				bufMonth[1] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryDate.Month), LSBCDIGIT);
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMonth[0], POINT_OFF, DOUBLEPOINT_OFF, 2);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufMonth[1], POINT_ON, DOUBLEPOINT_OFF, 3);
				displayMustBeUpdated = false;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_YEAR:

			if (displayMustBeUpdated)
			{
				bufYear[0] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryDate.Year), MSBCDIGIT);
				bufYear[1] = convert_BCD_to_ASCII(RTC_ByteToBcd2(binaryDate.Year), LSBCDIGIT);
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufYear[0], POINT_OFF, DOUBLEPOINT_OFF, 4);
				BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufYear[1], POINT_ON, DOUBLEPOINT_OFF, 5);
				displayMustBeUpdated = false;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SAVE_DATE:

			theDate.Date = RTC_ByteToBcd2(binaryDate.Date);
			theDate.Month = RTC_ByteToBcd2(binaryDate.Month);
			theDate.Year = RTC_ByteToBcd2(binaryDate.Year);
			HAL_RTC_SetDate(&hrtc, &theDate, RTC_FORMAT_BCD);
			BSP_LCD_GLASS_Clear();
			BSP_LCD_GLASS_DisplayString((uint8_t*) "SAVED");
			HAL_Delay(2000);
			BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
			AppStatus = STATE_DISPLAY_DATE;
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_GET_DAY:

			BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_ALLSEG_ALLCOM, LCD_BLINKFREQUENCY_DIV512);

			HAL_RTC_GetTime(&hrtc, &theTime, RTC_FORMAT_BCD);
			HAL_RTC_GetDate(&hrtc, &theDate, RTC_FORMAT_BCD);
			weekDayNbr = theDate.WeekDay;
			AppStatus = STATE_SET_DAY;
			displayMustBeUpdated = true;
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_DAY:

			if (displayMustBeUpdated)
			{
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayString((uint8_t*) weekDay[weekDayNbr - 1]); /* WeekDay is in [1...7] ! */
				displayMustBeUpdated = false;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SAVE_DAY:

			theDate.WeekDay = weekDayNbr;
			HAL_RTC_SetDate(&hrtc, &theDate, RTC_FORMAT_BCD);
			BSP_LCD_GLASS_Clear();
			BSP_LCD_GLASS_DisplayString((uint8_t*) "SAVED");
			HAL_Delay(2000);
			BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
			AppStatus = STATE_DISPLAY_DAY;
			break;

			/*-------------------------------------------------------------------------------------*/
		}
	}

	/******************************* Now, go to standby mode *********************/
	/******************************* Wake up with joystick center button **********/
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

	//BSP_QSPI_DeInit();
	//BSP_GYRO_DeInit();

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
 * @brief EXTI line detection callbacks : manages transitions between application states and small actions.
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	inactivity_time = 0;
	//bLCDGlass_KeyPressed = 0x01;
	displayMustBeUpdated = true;

	if ((GPIO_Pin & (DOWN_JOY_PIN | UP_JOY_PIN | SEL_JOY_PIN | RIGHT_JOY_PIN | LEFT_JOY_PIN)) != RESET)
	{
		switch (AppStatus)
		{
		/*-------------------------------------------------------------------------------------*/
		case STATE_DISPLAY_TIME:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				AppStatus = STATE_DISPLAY_DATE;
				break;

			case UP_JOY_PIN:
				AppStatus = STATE_DISPLAY_DAY;
				break;

			case RIGHT_JOY_PIN:
				AppStatus = STATE_GET_TIME;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/

		case STATE_DISPLAY_DATE:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				AppStatus = STATE_DISPLAY_DAY;
				break;

			case UP_JOY_PIN:
				AppStatus = STATE_DISPLAY_TIME;
				break;

			case RIGHT_JOY_PIN:
				AppStatus = STATE_GET_DATE;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/

		case STATE_DISPLAY_DAY:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				AppStatus = STATE_DISPLAY_TIME;
				break;

			case UP_JOY_PIN:
				AppStatus = STATE_DISPLAY_DATE;
				break;

			case RIGHT_JOY_PIN:
				AppStatus = STATE_GET_DAY;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/

		case STATE_SET_HOURS:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				if (binaryTime.Hours > 0)
					binaryTime.Hours--;
				else
					binaryTime.Hours = 23;
				break;

			case UP_JOY_PIN:
				if (binaryTime.Hours < 23)
					binaryTime.Hours++;
				else
					binaryTime.Hours = 0;
				break;

			case RIGHT_JOY_PIN: /*  */
				AppStatus = STATE_SET_MINS;
				break;

			case LEFT_JOY_PIN: /* exit without saving new time */
				BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
				AppStatus = STATE_DISPLAY_TIME;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/

		case STATE_SET_MINS:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				if (binaryTime.Minutes > 0)
					binaryTime.Minutes--;
				else
					binaryTime.Minutes = 59;
				break;

			case UP_JOY_PIN:
				if (binaryTime.Minutes < 59)
					binaryTime.Minutes++;
				else
					binaryTime.Minutes = 0;
				break;

			case RIGHT_JOY_PIN: /*  */
				AppStatus = STATE_SET_SECS;
				break;

			case LEFT_JOY_PIN: /* */
				AppStatus = STATE_SET_HOURS;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_SECS:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				if (binaryTime.Seconds > 0)
					binaryTime.Seconds--;
				else
					binaryTime.Seconds = 59;
				break;

			case UP_JOY_PIN:
				if (binaryTime.Seconds < 59)
					binaryTime.Seconds++;
				else
					binaryTime.Seconds = 0;
				break;

			case RIGHT_JOY_PIN: /*  */
				AppStatus = STATE_SAVE_TIME;
				break;

			case LEFT_JOY_PIN: /* */
				AppStatus = STATE_SET_MINS;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_DAY_NBR:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				if (binaryDate.Date > 1)
					binaryDate.Date--;
				else
					binaryDate.Date = 31;
				break;

			case UP_JOY_PIN:
				if (binaryDate.Date < 31)
					binaryDate.Date++;
				else
					binaryDate.Date = 1;
				break;

			case RIGHT_JOY_PIN: /*  */
				AppStatus = STATE_SET_MONTH;
				break;

			case LEFT_JOY_PIN: /* exit without saving new date */
				BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
				AppStatus = STATE_DISPLAY_DATE;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_MONTH:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				if (binaryDate.Month > 1)
					binaryDate.Month--;
				else
					binaryDate.Month = 12;
				break;

			case UP_JOY_PIN:
				if (binaryDate.Month < 12)
					binaryDate.Month++;
				else
					binaryDate.Month = 1;
				break;

			case RIGHT_JOY_PIN: /*  */
				AppStatus = STATE_SET_YEAR;
				break;

			case LEFT_JOY_PIN: /*  */
				//BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
				AppStatus = STATE_SET_DAY_NBR;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_YEAR:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				if (binaryDate.Year > 0)
					binaryDate.Year--;
				else
					binaryDate.Year = 99;
				break;

			case UP_JOY_PIN:
				if (binaryDate.Year < 99)
					binaryDate.Year++;
				else
					binaryDate.Year = 0;
				break;

			case RIGHT_JOY_PIN: /*  */
				AppStatus = STATE_SAVE_DATE;
				break;

			case LEFT_JOY_PIN: /*  */
				//BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
				AppStatus = STATE_SET_MONTH;
				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_SET_DAY:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				if (weekDayNbr < 7)
					weekDayNbr++;
				else
					weekDayNbr = 1;
				break;

			case UP_JOY_PIN:
				if (weekDayNbr > 1)
					weekDayNbr--;
				else
					weekDayNbr = 7;
				break;

			case RIGHT_JOY_PIN: /* save new day and exit */
				AppStatus = STATE_SAVE_DAY;
				break;

			case LEFT_JOY_PIN: /* exit without saving new day */
				BSP_LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
				AppStatus = STATE_DISPLAY_DAY;
				break;
			}
			break;

		}
	}
}

/**
 * @brief  Display first start messages
 * @retval None
 */
static void Display_First_Start_msg(void)
{
	/* Clear the LCD */
	BSP_LCD_GLASS_Clear();

	/* Display LCD messages */
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     *HORLOGEUSE*", 1,	SCROLL_SPEED_MEDIUM);
	HAL_Delay(50);
	BSP_LCD_GLASS_Clear();
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     PAR XAVIER HALGAND 2025", 1, SCROLL_SPEED_MEDIUM);
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     METTRE A L HEURE SVP", 1, SCROLL_SPEED_MEDIUM);
	BSP_LCD_GLASS_Clear();
}

/**
 * @brief  Display wake up message
 * @retval None
 */
static void Display_WakeUp_msg(void)
{
	/* Clear the LCD */
	BSP_LCD_GLASS_Clear();

	/* Display LCD messages */
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     JE ME REVEILLE", 1, SCROLL_SPEED_MEDIUM);
	HAL_Delay(50);
	BSP_LCD_GLASS_Clear();
}

/**
 * @brief  Converts a 4 bit BCD (upper or lower in a byte) into ASCII code
 * @param	digit : must be MSBCDIGIT or LSBCDIGIT (most significant BCD or less significant)
 * @retval ASCII code
 */
uint8_t convert_BCD_to_ASCII(uint8_t bcd_data, BCDigit_TypeDef digit)
{
	if (digit == MSBCDIGIT)
	{
		return (bcd_data >> 4) + 48;
	}
	if (digit == LSBCDIGIT)
	{
		return (bcd_data & 0x0F) + 48;
	}
	return 42; /* character '*' if unknown */

}
/********************************************************************************************/
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
