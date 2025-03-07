/*
 * retarget.h
 *
 *  Created on: 14 févr. 2023
 *      Author: XavSab
 */

#ifndef INC_RETARGET_H_
#define INC_RETARGET_H_

/*--------------------------------------------------------------*/

#include "main.h"
#include <sys/stat.h>
#include "usart.h"

/*---------------------------------------------------------------------------------------------*/
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif

void retargetInit(UART_HandleTypeDef *huart);
int _write(int fd, char* ptr, int len);
int _read(int fd, char* ptr, int len);

/*-----------------------------------------------------------------*/

#endif /* INC_RETARGET_H_ */
