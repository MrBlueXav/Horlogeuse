/*
 * STM32L476_DK_glass_lcd_extension.h
 *
 *  Created on: Mar 20, 2025
 *      Author: Xavier Halgand
 */

#ifndef BSP_STM32L476G_DISCOVERY_STM32L476_DK_GLASS_LCD_EXTENSION_H_
#define BSP_STM32L476G_DISCOVERY_STM32L476_DK_GLASS_LCD_EXTENSION_H_
/************************************************************************************************/

#include "stm32l476g_discovery_glass_lcd.h"

/* -------------------------------------------------------------------------------*/
typedef enum
{
  LCD_DOT_0 = 0, /* first dot at left */
  LCD_DOT_1 = 1,
  LCD_DOT_2 = 2,
  LCD_DOT_3 = 3,
}
Dot_Typedef;

typedef enum
{
  LCD_COLON_0 = 0, /* first colon at left */
  LCD_COLON_1 = 1,
  LCD_COLON_2 = 2,
  LCD_COLON_3 = 3,
}
Colon_Typedef;


/* Exported functions --------------------------------------------------------*/
void LCD_GLASS_Display_Dot(Dot_Typedef dot);
void LCD_GLASS_Display_Colon(Colon_Typedef colon);
void BSP_LCD_GLASS_DisplayString_plus_one_dot(uint8_t *ptr, DigitPosition_Typedef pos);
void BSP_LCD_GLASS_DisplayChar2(uint8_t *ch, Point_Typedef Point, DoublePoint_Typedef Colon, DigitPosition_Typedef Position);
void BSP_LCD_GLASS_DisplayString2(uint8_t *ptr);
void BSP_LCD_GLASS_ScrollSentence2(uint8_t *ptr, uint16_t nScroll, uint16_t ScrollSpeed);
void Dot_colon_LCD_test(void);
void print_all_characters(void);

/************************************************************************************************/
#endif /* BSP_STM32L476G_DISCOVERY_STM32L476_DK_GLASS_LCD_EXTENSION_H_ */
