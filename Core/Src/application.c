/*
 * application.c
 *
 * 		L'HORLOGEUSE L476
 *
 *  Created on: Mar 1, 2025
 *  14/03/25
 *      Author: Xavier Halgand
 */

/* Includes ------------------------------------------------------------------*/
#include "application.h"
#include "main.h"
#include "SHT45_appli.h"
#include <stdbool.h>

/********************************************* Global variables *****************************************************/
extern RTC_HandleTypeDef hrtc;
extern RNG_HandleTypeDef hrng;
extern volatile uint8_t bLCDGlass_KeyPressed;
extern __IO uint32_t AppStatus;
extern __IO uint32_t inactivity_time;
extern __IO uint32_t time_counter;
extern __IO uint32_t time_counter2;

volatile bool displayMustBeUpdated = true;

const char *weekDay[] =
{ "LUNDI", "MARDI", "MERCRE", "JEUDI", "VENDRE", "SAMEDI", "DIMAN" }; // in french
uint8_t weekDayNbr;
RTC_TimeTypeDef theTime;
RTC_TimeTypeDef binaryTime;
RTC_DateTypeDef theDate;
RTC_DateTypeDef binaryDate;

uint32_t diceResult = 0;
uint8_t diceResultPosition = 0;
volatile bool diceDisplayMustBeUpdated = true;

/*------------------------------------------------------------------------------------------------*/
void Display_dice_intro(void)
{
	BSP_LCD_GLASS_Clear();
	BSP_LCD_GLASS_ScrollSentence2((uint8_t*) "     *DICE*", 1, SCROLL_SPEED_MEDIUM);
}

/*------------------------------------------------------------------------------------------------*/
uint32_t Get_dice(void)
{
	uint32_t aRandom32bit = 0;
	if (HAL_RNG_GenerateRandomNumber(&hrng, &aRandom32bit) != HAL_OK)
	{
		/* Random number generation error */
		Error_Handler();
	}
	return (aRandom32bit % 6 + 1);
}

/**************************************************************************************************/
void application(void)
{
	uint8_t bufSec[2];
	uint8_t bufMin[2];
	uint8_t bufHour[2];
	uint8_t bufDate[2];
	uint8_t bufMonth[2];
	uint8_t bufYear[2];

	if (HAL_GetTick() - time_counter2 > 50) // refresh LCD every 50ms
	{
		time_counter2 = HAL_GetTick();
		displayMustBeUpdated = true;
	}

	switch (AppStatus)
	{
	/*-------------------------------------------------------------------------------------*/

		/*-------------------------------------------------------------------------------------*/
	case STATE_DISPLAY_TEMPERATURE:

		if (HAL_GetTick() - time_counter > 1000)
		{
			time_counter = HAL_GetTick();
			//inactivity_time = 0; /* prevent auto standby */
			SHT45_LCD_temperature_display();
		}
		break;

		/*-------------------------------------------------------------------------------------*/
	case STATE_DISPLAY_HUMIDITY:

		if (HAL_GetTick() - time_counter > 1000)
		{
			time_counter = HAL_GetTick();
			//inactivity_time = 0; /* prevent auto standby */
			SHT45_LCD_humidity_display();
		}
		break;


		/*-------------------------------------------------------------------------------------*/
	}
}

/**
 * @brief Application EXTI line detection callback : manages transitions between application states and small actions.
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void application_JOY_callback(uint16_t GPIO_Pin)
{

	inactivity_time = 0;
	displayMustBeUpdated = true;

	if ((GPIO_Pin & (DOWN_JOY_PIN | UP_JOY_PIN | SEL_JOY_PIN | RIGHT_JOY_PIN | LEFT_JOY_PIN)) != RESET)
	{
		switch (AppStatus)
		{
		/*-------------------------------------------------------------------------------------*/
		case STATE_START:

			bLCDGlass_KeyPressed = 0x01; // to skip scrolling messages at start
			break;


			/*-------------------------------------------------------------------------------------*/
		case STATE_DISPLAY_TEMPERATURE:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				AppStatus = STATE_DISPLAY_HUMIDITY;
				break;

			case UP_JOY_PIN:
				AppStatus = STATE_DISPLAY_HUMIDITY;
				break;

//			case RIGHT_JOY_PIN:
//				AppStatus = STATE_GET_DAY;
//				break;
			}
			break;

			/*-------------------------------------------------------------------------------------*/
		case STATE_DISPLAY_HUMIDITY:

			switch (GPIO_Pin)
			{
			case DOWN_JOY_PIN:
				AppStatus = STATE_DISPLAY_TEMPERATURE;
				break;

			case UP_JOY_PIN:
				AppStatus = STATE_DISPLAY_TEMPERATURE;
				break;

				//			case RIGHT_JOY_PIN:
				//				AppStatus = STATE_GET_DAY;
				//				break;
			}
			break;

		}
	}
}

/**
 * @brief  Display first start messages
 * @retval None
 */
void Display_First_Start_msg(void)
{
	/* Clear the LCD */
	BSP_LCD_GLASS_Clear();

	/* Display LCD messages */
	BSP_LCD_GLASS_ScrollSentence2((uint8_t*) "     *HORLOGEUSE*", 1, SCROLL_SPEED_MEDIUM);
	HAL_Delay(50);
	BSP_LCD_GLASS_Clear();
	BSP_LCD_GLASS_ScrollSentence2((uint8_t*) "     Par Xavier HALGAND 2025", 1, SCROLL_SPEED_MEDIUM);
	BSP_LCD_GLASS_ScrollSentence2((uint8_t*) "     Mettre a l'heure SVP", 1, SCROLL_SPEED_MEDIUM);
	BSP_LCD_GLASS_Clear();
}

/**
 * @brief  Display wake up message
 * @retval None
 */
void Display_WakeUp_msg(void)
{
	/* Clear the LCD */
	BSP_LCD_GLASS_Clear();

	/* Display LCD messages */

	//BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     JE ME REVEILLE", 1, SCROLL_SPEED_MEDIUM);
	BSP_LCD_GLASS_DisplayString2((uint8_t*) "Coucou");
	HAL_Delay(2000);

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
