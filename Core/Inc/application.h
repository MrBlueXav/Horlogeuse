/*
 * application.h
 *
 *  Created on: Mar 1, 2025
 *      Author: XavSab
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

/***********************************************************************************************************/
#include "stm32l476g_discovery.h"
#include "stm32l476g_discovery_glass_lcd.h"

/***********************************************************************************************************/
typedef enum
{
	STATE_START, /*  */
	STATE_DISPLAY_TIME, /*  */
	STATE_DISPLAY_DATE, /*  */
	STATE_DISPLAY_DAY, /*  */
	STATE_DISPLAY_TEMPERATURE, /*  */
	STATE_DISPLAY_HUMIDITY, /*  */
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

/***********************************************************************************************************/
void application(void);
void application_JOY_callback(uint16_t GPIO_Pin);
void Display_First_Start_msg(void);
void Display_WakeUp_msg(void);
uint8_t convert_BCD_to_ASCII(uint8_t bcd_data, BCDigit_TypeDef digit);

/***********************************************************************************************************/

#endif /* INC_APPLICATION_H_ */

