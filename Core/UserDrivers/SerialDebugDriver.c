/*
 * SerialDebugDriver.c
 *
 *  Created on: Sep. 5, 2022
 *      Author: mingy
 */

#include "SerialDebugDriver.h"

//extern UART_HandleTypeDef huart2;
static char messageBuf[MAX_SERIAL_PRINT_LENGTH];

PUBLIC void SerialPrint(const char * message, ...)
{
//#ifdef VERBOSE
	va_list args;
	va_start(args, message);
	length_t len = vsprintf(messageBuf, message, args);
//	HAL_UART_Transmit(&huart2, (uint8_t*)messageBuf, len, HAL_MAX_DELAY);
	va_end(args);
//#endif
}
PUBLIC void SerialPrintln(const char * message, ...)
{
//#ifdef VERBOSE
	va_list args;
	va_start(args, message);
	length_t len = vsprintf(messageBuf, message, args);
	messageBuf[len] = '\n';
	messageBuf[len+1] = '\r';
//	HAL_UART_Transmit(&huart2, (uint8_t*)messageBuf, len+2, HAL_MAX_DELAY);
	va_end(args);
//#endif
}
