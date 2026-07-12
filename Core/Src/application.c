/*
 * application.c
 *
 * 		THERMOmètre L476
 *
 *  July 2026
 *  Author: Xavier Halgand
 */

/* Includes ------------------------------------------------------------------*/
#include "application.h"
#include "main.h"
#include "SHT45_appli.h"
#include <stdbool.h>

/********************************************* Global variables *****************************************************/
extern RTC_HandleTypeDef hrtc;
extern RNG_HandleTypeDef hrng;
extern __IO uint8_t bLCDGlass_KeyPressed;
extern __IO uint32_t AppStatus;
extern __IO uint32_t inactivity_time;
extern __IO uint32_t time_counter;
extern __IO uint32_t time_counter2;
extern __IO uint8_t eco_on;

volatile bool displayMustBeUpdated = true;

//const char *weekDay[] =
//{ "LUNDI", "MARDI", "MERCRE", "JEUDI", "VENDRE", "SAMEDI", "DIMAN" }; // in french
//uint8_t weekDayNbr;
//RTC_TimeTypeDef theTime;
//RTC_TimeTypeDef binaryTime;
//RTC_DateTypeDef theDate;
//RTC_DateTypeDef binaryDate;

/**************************************************************************************************/
void application(void)
{
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

			case RIGHT_JOY_PIN:
				eco_on = 1;
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayString2((uint8_t*) "Eco ON");
				//HAL_Delay(2000); // will block !
				break;

			case LEFT_JOY_PIN:
				eco_on = 0;
				BSP_LCD_GLASS_Clear();
				BSP_LCD_GLASS_DisplayString2((uint8_t*) "EcoOFF");
				//HAL_Delay(2000); //will block !
				break;
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
			}
			break;

		}
	}
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
