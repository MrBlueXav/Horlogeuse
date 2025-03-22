/*
 * STM32L476_DK_glass_lcd_extension.c
 *
 *	********Text encoding must be ISO-8859-1 ! *******
 *
 *  Created on: Mar 20, 2025
 *      Author: Xavier Halgand
 */

/* Includes ------------------------------------------------------------------*/
#include "STM32L476_DK_glass_lcd_extension.h"

/* Global extern variables ---------------------------------------------------*/
extern LCD_HandleTypeDef LCDHandle;
extern const uint16_t CapLetterMap[26];
extern const uint16_t NumberMap[10];
extern uint32_t Digit[4];
extern uint8_t LCDBar;
extern const uint16_t FourteenSegmentASCII[96];

/* Defines -------------------------------------------------------------------*/
#define ASCII_CHAR_0                  0x30  /* 0 */
#define ASCII_CHAR_AT_SYMBOL          0x40  /* @ */
#define ASCII_CHAR_LEFT_OPEN_BRACKET  0x5B  /* [ */
#define ASCII_CHAR_APOSTROPHE         0x60  /* ` */
#define ASCII_CHAR_LEFT_OPEN_BRACE    0x7B  /* ( */

/**************************************************************************************************************/
/**
 * @brief  Convert an ascii char to the a LCD digit.
 * @param  Char: a char to display.
 * @param  Point: a point to add in front of char
 *         This parameter can be: POINT_OFF or POINT_ON
 * @param  Colon : flag indicating if a colon character has to be added in front
 *         of displayed character.
 *         This parameter can be: DOUBLEPOINT_OFF or DOUBLEPOINT_ON.
 * @retval None
 */
static void Convert(uint8_t *Char, Point_Typedef Point, DoublePoint_Typedef Colon)
{
	uint16_t ch = 0;
	uint8_t loop = 0, index = 0;

	switch (*Char)
	{
	case ' ':
		ch = 0x00;
		break;

	case '*':
		ch = C_STAR;
		break;

	case '(':
		ch = C_OPENPARMAP;
		break;

	case ')':
		ch = C_CLOSEPARMAP;
		break;

	case 'd':
		ch = C_DMAP;
		break;

	case 'm':
		ch = C_MMAP;
		break;

	case 'n':
		ch = C_NMAP;
		break;

	case 'µ':
		ch = C_UMAP;
		break;

	case '-':
		ch = C_MINUS;
		break;

	case '+':
		ch = C_PLUS;
		break;

	case '/':
		ch = C_SLATCH;
		break;

	case '°':
		ch = C_PERCENT_1;
		break;

	case '%':
		ch = C_PERCENT_2;
		break;

	case 255:
		ch = C_FULL;
		break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		ch = NumberMap[*Char - ASCII_CHAR_0];
		break;

	default:
		/* The character Char is one letter in upper case*/
		if ((*Char < ASCII_CHAR_LEFT_OPEN_BRACKET) && (*Char > ASCII_CHAR_AT_SYMBOL))
		{
			ch = CapLetterMap[*Char - 'A'];
		}
		/* The character Char is one letter in lower case*/
		if ((*Char < ASCII_CHAR_LEFT_OPEN_BRACE) && (*Char > ASCII_CHAR_APOSTROPHE))
		{
			ch = CapLetterMap[*Char - 'a'];
		}
		break;
	}

	/* Set the digital point can be displayed if the point is on */
	if (Point == POINT_ON)
	{
		ch |= 0x0002;
	}

	/* Set the "COL" segment in the character that can be displayed if the colon is on */
	if (Colon == DOUBLEPOINT_ON)
	{
		ch |= 0x0020;
	}

	for (loop = 12, index = 0; index < 4; loop -= 4, index++)
	{
		Digit[index] = (ch >> loop) & 0x0f; /*To isolate the less significant digit */
	}
}

/*----------------------------------------------------------------------------*/
uint16_t convert_to_STM32L476DK_glass_LCD(uint16_t code)
{
	uint16_t converted =
			((0x0001 & code) << 10) |
			((0x0002 & code) << 13) |
			((0x0004 & code) <<  7) |
			((0x0008 & code) <<  5) |
			((0x0010 & code) <<  8) |
			((0x0020 & code) <<  6) |
			((0x0040 & code) <<  9) |
			((0x0080 & code) <<  6) |
			((0x0100 & code) >>  5) |
			((0x0200 & code) >>  7) |
			((0x0400 & code) >>  4) |
			((0x0800 & code) >>  4) |
			((0x1000 & code) >>  8) |
			((0x2000 & code) >> 13) |
			((0x4000 & code) >> 13) |
			((0x8000 & code) >> 10);
	return converted;
}

/*----------------------------------------------------------------------------*/
static void Convert2(uint8_t *Char, Point_Typedef Point, DoublePoint_Typedef Colon)
{
	uint16_t ch = 0;
	uint8_t loop = 0, index = 0;

	switch (*Char)
	{

	case 'µ':
		ch = C_UMAP;
		break;

	case '°':
		ch = C_PERCENT_1;
		break;

	default:
		/* The character Char is in ASCII table */
		if ((*Char < 127) && (*Char > 31))
		{
			ch = convert_to_STM32L476DK_glass_LCD(FourteenSegmentASCII[*Char - 32]);
		}
		else ch = C_FULL; /* non printable character */
		break;
	}

	/* Set the digital point can be displayed if the point is on */
	if (Point == POINT_ON)
	{
		ch |= 0x0002;
	}

	/* Set the "COL" segment in the character that can be displayed if the colon is on */
	if (Colon == DOUBLEPOINT_ON)
	{
		ch |= 0x0020;
	}

	for (loop = 12, index = 0; index < 4; loop -= 4, index++)
	{
		Digit[index] = (ch >> loop) & 0x0f; /*To isolate the less significant digit */
	}
}

/*--------------------------------------------------------------------------------------------*/
/**
 * @brief  Write a character in the LCD frame buffer.
 * @param  ch: the character to display.
 * @param  Point: a point to add in front of char
 *         This parameter can be: POINT_OFF or POINT_ON
 * @param  Colon: flag indicating if a colon character has to be added in front
 *         of displayed character.
 *         This parameter can be: DOUBLEPOINT_OFF or DOUBLEPOINT_ON.
 * @param  Position: position in the LCD of the character to write [1:6]
 * @retval None
 */
static void WriteChar(uint8_t *ch, Point_Typedef Point, DoublePoint_Typedef Colon, DigitPosition_Typedef Position)
{
	uint32_t data = 0x00;
	/* To convert displayed character in segment in array digit */
	Convert(ch, (Point_Typedef) Point, (DoublePoint_Typedef) Colon);

	switch (Position)
	{
	/* Position 1 on LCD (Digit1)*/
	case LCD_DIGIT_POSITION_1:
		data = ((Digit[0] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM0, LCD_DIGIT1_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM1, LCD_DIGIT1_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM2, LCD_DIGIT1_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM3, LCD_DIGIT1_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 2 on LCD (Digit2)*/
	case LCD_DIGIT_POSITION_2:
		data = ((Digit[0] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM0, LCD_DIGIT2_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM1, LCD_DIGIT2_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM2, LCD_DIGIT2_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM3, LCD_DIGIT2_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 3 on LCD (Digit3)*/
	case LCD_DIGIT_POSITION_3:
		data = ((Digit[0] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM0, LCD_DIGIT3_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM1, LCD_DIGIT3_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM2, LCD_DIGIT3_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM3, LCD_DIGIT3_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 4 on LCD (Digit4)*/
	case LCD_DIGIT_POSITION_4:
		data = ((Digit[0] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM0, LCD_DIGIT4_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = (((Digit[0] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM0_1, LCD_DIGIT4_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM1, LCD_DIGIT4_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = (((Digit[1] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM1_1, LCD_DIGIT4_COM1_1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM2, LCD_DIGIT4_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = (((Digit[2] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM2_1, LCD_DIGIT4_COM2_1_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM3, LCD_DIGIT4_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */

		data = (((Digit[3] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM3_1, LCD_DIGIT4_COM3_1_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 5 on LCD (Digit5)*/
	case LCD_DIGIT_POSITION_5:
		data = (((Digit[0] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM0, LCD_DIGIT5_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[0] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM0_1, LCD_DIGIT5_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = (((Digit[1] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM1, LCD_DIGIT5_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[1] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM1_1, LCD_DIGIT5_COM1_1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = (((Digit[2] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM2, LCD_DIGIT5_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[2] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM2_1, LCD_DIGIT5_COM2_1_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = (((Digit[3] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM3, LCD_DIGIT5_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */

		data = ((Digit[3] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM3_1, LCD_DIGIT5_COM3_1_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 6 on LCD (Digit6)*/
	case LCD_DIGIT_POSITION_6:
		data = ((Digit[0] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM0, LCD_DIGIT6_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM1, LCD_DIGIT6_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM2, LCD_DIGIT6_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM3, LCD_DIGIT6_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

	default:
		break;
	}
}


static void WriteChar2(uint8_t *ch, Point_Typedef Point, DoublePoint_Typedef Colon, DigitPosition_Typedef Position)
{
	uint32_t data = 0x00;
	/* To convert displayed character in segment in array digit */
	Convert2(ch, (Point_Typedef) Point, (DoublePoint_Typedef) Colon);

	switch (Position)
	{
	/* Position 1 on LCD (Digit1)*/
	case LCD_DIGIT_POSITION_1:
		data = ((Digit[0] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM0, LCD_DIGIT1_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM1, LCD_DIGIT1_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM2, LCD_DIGIT1_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG1_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG23_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT1_COM3, LCD_DIGIT1_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 2 on LCD (Digit2)*/
	case LCD_DIGIT_POSITION_2:
		data = ((Digit[0] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM0, LCD_DIGIT2_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM1, LCD_DIGIT2_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM2, LCD_DIGIT2_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG3_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG21_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT2_COM3, LCD_DIGIT2_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 3 on LCD (Digit3)*/
	case LCD_DIGIT_POSITION_3:
		data = ((Digit[0] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM0, LCD_DIGIT3_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM1, LCD_DIGIT3_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM2, LCD_DIGIT3_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG5_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG19_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT3_COM3, LCD_DIGIT3_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 4 on LCD (Digit4)*/
	case LCD_DIGIT_POSITION_4:
		data = ((Digit[0] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM0, LCD_DIGIT4_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = (((Digit[0] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM0_1, LCD_DIGIT4_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM1, LCD_DIGIT4_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = (((Digit[1] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM1_1, LCD_DIGIT4_COM1_1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM2, LCD_DIGIT4_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = (((Digit[2] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM2_1, LCD_DIGIT4_COM2_1_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG17_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM3, LCD_DIGIT4_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */

		data = (((Digit[3] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG16_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT4_COM3_1, LCD_DIGIT4_COM3_1_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 5 on LCD (Digit5)*/
	case LCD_DIGIT_POSITION_5:
		data = (((Digit[0] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM0, LCD_DIGIT5_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[0] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM0_1, LCD_DIGIT5_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = (((Digit[1] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM1, LCD_DIGIT5_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[1] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM1_1, LCD_DIGIT5_COM1_1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = (((Digit[2] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM2, LCD_DIGIT5_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[2] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM2_1, LCD_DIGIT5_COM2_1_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = (((Digit[3] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG14_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM3, LCD_DIGIT5_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */

		data = ((Digit[3] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG15_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT5_COM3_1, LCD_DIGIT5_COM3_1_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

		/* Position 6 on LCD (Digit6)*/
	case LCD_DIGIT_POSITION_6:
		data = ((Digit[0] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[0] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM0, LCD_DIGIT6_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

		data = ((Digit[1] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[1] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM1, LCD_DIGIT6_COM1_SEG_MASK, data); /* 1F 1A 1C 1D  */

		data = ((Digit[2] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[2] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM2, LCD_DIGIT6_COM2_SEG_MASK, data); /* 1Q 1K 1Col 1P  */

		data = ((Digit[3] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG11_SHIFT)
				| (((Digit[3] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG13_SHIFT);
		HAL_LCD_Write(&LCDHandle, LCD_DIGIT6_COM3, LCD_DIGIT6_COM3_SEG_MASK, data); /* 1H 1J 1DP 1N  */
		break;

	default:
		break;
	}
}

/*--------------------------------------------------------------------------------------------*/
/**
 * @brief  Write a character in the LCD RAM buffer.
 * @param  ch: The character to display.
 * @param  Point: A point to add in front of char.
 *          This parameter can be one of the following values:
 *              @arg POINT_OFF: No point to add in front of char.
 *              @arg POINT_ON: Add a point in front of char.
 * @param  Colon: Flag indicating if a colon character has to be added in front
 *                     of displayed character.
 *          This parameter can be one of the following values:
 *              @arg DOUBLEPOINT_OFF: No colon to add in back of char.
 *              @arg DOUBLEPOINT_ON: Add an colon in back of char.
 * @param  Position: Position in the LCD of the character to write.
 *                   This parameter can be any value in range [1:6].
 * @retval None
 * @note   Required preconditions: The LCD should be cleared before to start the
 *         write operation.
 */
void BSP_LCD_GLASS_DisplayChar2(uint8_t *ch, Point_Typedef Point, DoublePoint_Typedef Colon,
		DigitPosition_Typedef Position)
{
	WriteChar2(ch, Point, Colon, Position);

	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
}

/*--------------------------------------------------------------------------------------------*/
/**
 * @brief  Write a character string in the LCD RAM buffer and a dot.
 * @param  ptr: Pointer to string to display on the LCD Glass.
 * @retval None
 */
void BSP_LCD_GLASS_DisplayString_plus_one_dot(uint8_t *ptr, DigitPosition_Typedef pos)
{
	DigitPosition_Typedef position = LCD_DIGIT_POSITION_1;

	/* Send the string character by character on lCD */
	while ((*ptr != 0) && (position <= LCD_DIGIT_POSITION_6))
	{
		/* Write one character on LCD */
		if (position == pos)
			WriteChar(ptr, POINT_ON, DOUBLEPOINT_OFF, position);
		else
			WriteChar(ptr, POINT_OFF, DOUBLEPOINT_OFF, position);

		/* Point on the next character */
		ptr++;

		/* Increment the character counter */
		position++;
	}
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
}

/*--------------------------------------------------------------------------------------------*/
void LCD_GLASS_Display_Dot(Dot_Typedef dot)
{
	switch (dot)
	{
	case LCD_DOT_0:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER6, ~LCD_SEG1, LCD_SEG1);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	case LCD_DOT_1:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER6, ~LCD_SEG3, LCD_SEG3);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	case LCD_DOT_2:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER6, ~LCD_SEG5, LCD_SEG5);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	case LCD_DOT_3:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER7, ~LCD_SEG7, LCD_SEG7);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	default:
		break;
	}
}

/*--------------------------------------------------------------------------------------------*/
void LCD_GLASS_Display_Colon(Colon_Typedef colon)
{
	switch (colon)
	{
	case LCD_COLON_0:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER4, ~LCD_SEG1, LCD_SEG1);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	case LCD_COLON_1:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER4, ~LCD_SEG3, LCD_SEG3);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	case LCD_COLON_2:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER4, ~LCD_SEG5, LCD_SEG5);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	case LCD_COLON_3:
		HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER5, ~LCD_SEG7, LCD_SEG7);
		/* Update the LCD display */
		HAL_LCD_UpdateDisplayRequest(&LCDHandle);
		break;

	default:
		break;
	}
}

/*--------------------------------------------------------------------------------------------*/
void Dot_colon_LCD_test0(void)
{
	BSP_LCD_GLASS_DisplayString((uint8_t*) "123456");
	HAL_Delay(1000);
	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER6, ~LCD_SEG1, LCD_SEG1);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);
	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER6, ~LCD_SEG3, LCD_SEG3);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);
	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER6, ~LCD_SEG5, LCD_SEG5);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);
	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER7, ~LCD_SEG7, LCD_SEG7);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);

	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER4, ~LCD_SEG1, LCD_SEG1);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);
	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER4, ~LCD_SEG3, LCD_SEG3);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);
	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER4, ~LCD_SEG5, LCD_SEG5);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);
	HAL_LCD_Write(&LCDHandle, LCD_RAM_REGISTER5, ~LCD_SEG7, LCD_SEG7);
	/* Update the LCD display */
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
	HAL_Delay(1000);
}

/*--------------------------------------------------------------------------------------------*/
void Dot_colon_LCD_test(void)
{
	BSP_LCD_GLASS_DisplayString((uint8_t*) "abcdef");
	HAL_Delay(1000);

	LCD_GLASS_Display_Dot(LCD_DOT_0);
	HAL_Delay(1000);
	LCD_GLASS_Display_Dot(LCD_DOT_1);
	HAL_Delay(1000);
	LCD_GLASS_Display_Dot(LCD_DOT_2);
	HAL_Delay(1000);
	LCD_GLASS_Display_Dot(LCD_DOT_3);
	HAL_Delay(1000);

	LCD_GLASS_Display_Colon(LCD_COLON_0);
	HAL_Delay(1000);
	LCD_GLASS_Display_Colon(LCD_COLON_1);
	HAL_Delay(1000);
	LCD_GLASS_Display_Colon(LCD_COLON_2);
	HAL_Delay(1000);
	LCD_GLASS_Display_Colon(LCD_COLON_3);
	HAL_Delay(1000);
}

/*--------------------------------------------------------------------------------------------*/
void print_all_characters(void)
{
	BSP_LCD_GLASS_Clear();
	uint8_t pos = 0;

	BSP_LCD_GLASS_DisplayChar2((uint8_t*) "µ", POINT_OFF, DOUBLEPOINT_OFF, pos);
	pos++;
	HAL_Delay(1000);
	BSP_LCD_GLASS_DisplayChar2((uint8_t*) "°", POINT_OFF, DOUBLEPOINT_OFF, pos);
	pos++;
	HAL_Delay(1000);
	BSP_LCD_GLASS_DisplayChar2((uint8_t*) "à", POINT_OFF, DOUBLEPOINT_OFF, pos); /* Non printable character => C_FULL */
	pos++;
	HAL_Delay(1000);

	for (uint8_t ch = 32; ch < 127; ch++)
	{
		BSP_LCD_GLASS_DisplayChar2(&ch, POINT_OFF, DOUBLEPOINT_OFF, pos);
		HAL_Delay(1000);
		pos++;
		if (pos >= 6)
			{
				pos = 0;
				BSP_LCD_GLASS_Clear();
			}
	}
}
