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
#include <stdbool.h>

/********************************************* Global variables *****************************************************/
extern RTC_HandleTypeDef hrtc;
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

/*****************************************************************************************************************/
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
		//binaryTime.Seconds = RTC_Bcd2ToByte(theTime.Seconds);
		binaryTime.Seconds = 0x00;
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
			BSP_LCD_GLASS_DisplayChar((uint8_t*) &bufYear[1], POINT_OFF, DOUBLEPOINT_OFF, 5);
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
void Display_First_Start_msg(void)
{
	/* Clear the LCD */
	BSP_LCD_GLASS_Clear();

	/* Display LCD messages */
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     *HORLOGEUSE*", 1, SCROLL_SPEED_MEDIUM);
	HAL_Delay(50);
	BSP_LCD_GLASS_Clear();
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     *PAR XAVIER HALGAND 2025", 1, SCROLL_SPEED_MEDIUM);
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     *METTRE A L HEURE SVP", 1, SCROLL_SPEED_MEDIUM);
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
	BSP_LCD_GLASS_ScrollSentence((uint8_t*) "     *JE ME REVEILLE", 1, SCROLL_SPEED_MEDIUM);
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
