/*
 * SPIDriver.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef USERDRIVERS_SPIMOTORDRIVER_H_
#define USERDRIVERS_SPIMOTORDRIVER_H_

// STM headers
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"

// TMC headers
#include "tmc/ic/TMC4671/TMC4671.h"

#define TMC4671_CS 0
#define TMC6200_CS 1
#define TMC6200_EEPROM_1_CS 2
#define TMC6200_EEPROM_2_CS 3

uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer);

void initMotor();

void rotateMotorRight();

void rotateMotorLeft();

void stopMotor();

void setCS(uint8_t cs, GPIO_PinState state);

#endif /* USERDRIVERS_SPIMOTORDRIVER_H_ */
