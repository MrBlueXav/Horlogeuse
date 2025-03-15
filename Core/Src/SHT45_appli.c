/*
 * SHT45_appli.c
 *
 *  Created on: Mar 6, 2025
 *      Author: XavSab
 */
/***********************************************************************/
#include "stm32l476g_discovery.h"
#include "SHT45_appli.h"
#include "retarget.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "sht4x_i2c.h"

/***********************************************************************/
void LCD_temp_disp(int32_t temp)
{
	char LCDbuffer[10];
	sprintf(LCDbuffer, "%+06ld", temp);
	BSP_LCD_GLASS_Clear();

	if (temp >= 100000l)
		BSP_LCD_GLASS_DisplayString_plus_one_dot((uint8_t*) LCDbuffer, LCD_DIGIT_POSITION_4);
	else
		BSP_LCD_GLASS_DisplayString_plus_one_dot((uint8_t*) LCDbuffer, LCD_DIGIT_POSITION_3);

}

void SHT45_LCD_temperature_display(void)
{
	char LCDbuffer[10];
	int16_t error = NO_ERROR;
	int32_t temperature_milli_degC = 0;
	int32_t humidity_milli_RH = 0;

	error = sht4x_measure_high_precision(&temperature_milli_degC, &humidity_milli_RH);
	if (error != NO_ERROR)
	{
		//printf("error executing measure : %i\n", error);
		BSP_LCD_GLASS_Clear();
		BSP_LCD_GLASS_DisplayString((uint8_t*) "ERROR");
	}
	else
	{
		LCD_temp_disp(temperature_milli_degC);
	}
}

void SHT45_LCD_humidity_display(void)
{
	char LCDbuffer[10];
	int16_t error = NO_ERROR;
	int32_t temperature_milli_degC = 0;
	int32_t humidity_milli_RH = 0;

	error = sht4x_measure_high_precision(&temperature_milli_degC, &humidity_milli_RH);
	if (error != NO_ERROR)
	{
		//printf("error executing measure : %i\n", error);
		BSP_LCD_GLASS_Clear();
		BSP_LCD_GLASS_DisplayString((uint8_t*) "ERROR");
	}
	else
	{
		sprintf(LCDbuffer, "%ldH", humidity_milli_RH);
		BSP_LCD_GLASS_Clear();
		BSP_LCD_GLASS_DisplayString_plus_one_dot((uint8_t*) LCDbuffer, LCD_DIGIT_POSITION_2);
	}
}

void SHT45_LCD_test(void)
{
	char LCDbuffer[10];
	int16_t error = NO_ERROR;
	int32_t temperature_milli_degC = 0;
	int32_t humidity_milli_RH = 0;

	sensirion_i2c_hal_init(&hi2c1);
	sht4x_init(SHT40_I2C_ADDR_44);
	sht4x_soft_reset();
	HAL_Delay(20);

	LCD_temp_disp(110000l);
	HAL_Delay(4000);

	LCD_temp_disp(31786l);
	HAL_Delay(4000);

	LCD_temp_disp(-5471l);
	HAL_Delay(4000);

	LCD_temp_disp(9098l);
	HAL_Delay(4000);

	LCD_temp_disp(46l);
	HAL_Delay(4000);

	LCD_temp_disp(-52l);
	HAL_Delay(4000);

	error = sht4x_measure_high_precision(&temperature_milli_degC, &humidity_milli_RH);
	if (error != NO_ERROR)
	{
		//printf("error executing measure : %i\n", error);
		BSP_LCD_GLASS_Clear();
		BSP_LCD_GLASS_DisplayString((uint8_t*) "ERROR");
		HAL_Delay(4000);
	}
	else
	{
		//float temperature = temperature_milli_degC / 1000.0f;
		LCD_temp_disp(temperature_milli_degC);
		HAL_Delay(4000);
		sprintf(LCDbuffer, "%ldH", humidity_milli_RH);
		BSP_LCD_GLASS_Clear();
		BSP_LCD_GLASS_DisplayString_plus_one_dot((uint8_t*) LCDbuffer, LCD_DIGIT_POSITION_2);
		HAL_Delay(4000);
	}

}

void SHT45_UART_test(void)
{
	retargetInit(&huart2);

	printf("\n\r");
	printf("******************************************\n\r");
	printf("Coucou c'est moi !\n\r");
	printf("******************************************\n\r");

	int16_t error = NO_ERROR;
	sensirion_i2c_hal_init(&hi2c1);
	sht4x_init(SHT40_I2C_ADDR_44);
	sht4x_soft_reset();
//sensirion_i2c_hal_sleep_usec(10000);
	HAL_Delay(500);

	uint32_t serial_number = 0;
	error = sht4x_serial_number(&serial_number);
	if (error != NO_ERROR)
	{
		printf("error executing serial_number(): %i\n", error);
		//return error;
	}
	printf("serial_number: %lu\n", serial_number);

	int32_t temperature_milli_degC = 0;
	int32_t humidity_milli_RH = 0;
	//BSP_LED_Toggle(LED2);
	//sensirion_i2c_hal_sleep_usec(20000);
	HAL_Delay(1000);

	error = sht4x_measure_high_precision(&temperature_milli_degC, &humidity_milli_RH);
	if (error != NO_ERROR)
	{
		printf("error executing measure : %i\n", error);
	}
	printf("Temperature milli °C: %li ----- ", temperature_milli_degC);
	printf("Humidity milli percent RH: %li\n", humidity_milli_RH);
}
