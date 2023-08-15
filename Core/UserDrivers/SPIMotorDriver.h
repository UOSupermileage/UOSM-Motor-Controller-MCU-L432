/*
 * SPIDriver.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef USERDRIVERS_SPIMOTORDRIVER_H_
#define USERDRIVERS_SPIMOTORDRIVER_H_

// STM headers
#include "stm32l4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"

#define TMC4671_CS 0
#define TMC6200_CS 1

typedef struct {
        velocity_t rpm_target;
        uint16_t acceleration;
} ramp_point_t;

/**
 * Read Write a single byte over SPI.
 * CS is held low until function is called with lastTransfer == 12.
 */
PUBLIC uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer);

/**
 * Set the Chip Select (CS) pin for a particular slave to state
 */
PRIVATE void MotorSetCS(uint8_t cs, GPIO_PinState state);

PUBLIC uint32_t MotorValidateSPI();

// ===== Functions for interacting with motor =====
/**
 * Rotate the motor
 */
PUBLIC uint8_t MotorRotateVelocity(velocity_t velocity);

PUBLIC uint8_t MotorPeriodicJob();

PUBLIC uint8_t MotorEnableDriver(Enable_t enabled);

PUBLIC velocity_t MotorGetActualVelocity();
/**
 * Initialize registers in the TMC4671 and TMC6200. Must be called before motor
 * will function properly. Returns true if motor init was successful.
 */
PUBLIC uint8_t MotorInit();

PUBLIC uint8_t MotorDeInit();

PUBLIC uint8_t MotorInitEncoder();

PUBLIC void MotorHealth();

PUBLIC void MotorClearChargePump();

PUBLIC void MotorPrintFaults();

#endif /* USERDRIVERS_SPIMOTORDRIVER_H_ */
