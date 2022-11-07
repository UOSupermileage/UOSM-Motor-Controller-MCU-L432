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

/**
 * Read Write a single byte over SPI.
 * CS is held low until function is called with lastTransfer == 12.
 */
PUBLIC uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer);

/**
 * Set the Chip Select (CS) pin for a particular slave to state
 */
PRIVATE void setCS(uint8_t cs, GPIO_PinState state);

/**
 * Initialize registers in the TMC4671 and TMC6200. Must be called before motor will function properly.
 */
PUBLIC void initMotor();

/**
 * Set the target torque of the motor.
 */
PUBLIC void setTargetTorque(uint32_t torque);


#endif /* USERDRIVERS_SPIMOTORDRIVER_H_ */
