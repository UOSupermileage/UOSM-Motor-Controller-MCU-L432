/*
 * SerialDebugDriver.c
 *
 *  Created on: Sep. 5, 2022
 *      Author: mingy
 */

#include "SerialDebugDriver.h"

#ifdef STM
static char messageBuf[MAX_SERIAL_PRINT_LENGTH];

extern UART_HandleTypeDef DEBUG_UART;

void SerialPrint(const char * message, ...)
{
	va_list args;
	va_start(args, message);
	uint16_t len = vsprintf(messageBuf, message, args);
	HAL_UART_Transmit(&DEBUG_UART, (uint8_t*)messageBuf, len, HAL_MAX_DELAY);
	va_end(args);
}
void SerialPrintln(const char * message, ...)
{
	va_list args;
	va_start(args, message);
	uint16_t len = vsprintf(messageBuf, message, args);
	messageBuf[len] = '\n';
	messageBuf[len+1] = '\r';
	HAL_UART_Transmit(&DEBUG_UART, (uint8_t*)messageBuf, len+2, HAL_MAX_DELAY);
	va_end(args);
}
#else
void SerialPrint(const char * message, ...) {
    va_list args;
    va_start(args, message);
    ExternalSerialPrint(message, args);
    va_end(args);
}
void SerialPrintln(const char * message, ...) {
    va_list args;
    va_start(args, message);
    ExternalSerialPrintln(message, args);
    va_end(args);
}
#endif