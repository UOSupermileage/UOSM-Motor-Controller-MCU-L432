/*
 * SerialDebugDriver.h
 *
 *  Created on: Sep. 5, 2022
 *      Author: mingy
 */

#ifndef USERDRIVERS_SERIALDEBUGDRIVER_H_
#define USERDRIVERS_SERIALDEBUGDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "UOSMCoreConfig.h"

#include <stdio.h>

// Standard Lib
#include <stdarg.h>
#include <string.h>

void SerialPrint(const char* message, ...);

void SerialPrintln(const char* message, ...);

#ifdef __cplusplus
}
#endif

#endif /* USERDRIVERS_SERIALDEBUGDRIVER_H_ */
