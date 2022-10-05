/*
 * SerialDebugDriver.h
 *
 *  Created on: Sep. 5, 2022
 *      Author: mingy
 */

#ifndef USERDRIVERS_SERIALDEBUGDRIVER_H_
#define USERDRIVERS_SERIALDEBUGDRIVER_H_

// STM headers
#include "stm32f4xx_hal.h"

// Standard Lib
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// Our headers
#include "ApplicationTypes.h"

#define MAX_SERIAL_PRINT_LENGTH 128

/* NOTE:
 * Initalization of the serial peripheral is done in main.c
 * Auto generated init by the IOC file */

PUBLIC void SerialPrint(const char * message, ...);
PUBLIC void SerialPrintln(const char * message, ...);

#endif /* USERDRIVERS_SERIALDEBUGDRIVER_H_ */
