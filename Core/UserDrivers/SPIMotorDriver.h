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
#include "cmsis_os.h"

// Our headers
#include "ApplicationTypes.h"

#define TMC4671_CS 0

typedef struct {
        velocity_t rpm_target;
        uint16_t acceleration;
} ramp_point_t;

typedef struct {
    MotorCode id;
    uint32_t pi;
    velocity_t maxVelocity;
    ramp_point_t* rampPoints;
    uint8_t rampSize;
} motor_t;

/**
 * Read Write a single byte over SPI.
 * CS is held low until function is called with lastTransfer == 12.
 */
uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer);

/**
 * Set the Chip Select (CS) pin for a particular slave to state
 */
void MotorSetCS(uint8_t cs, GPIO_PinState state);

/**
 * Set which motor is being driven
 * @param motor
 */
void MotorSelect(MotorCode motor);

uint32_t MotorValidateSPI();

// ===== Functions for interacting with motor =====
/**
 * Rotate the motor
 */
uint8_t MotorRotateVelocity(velocity_t velocity);

uint8_t MotorPeriodicJob();

uint8_t MotorEnableDriver(flag_status_t enabled);

velocity_t MotorGetActualVelocity();
/**
 * Initialize registers in the TMC4671 and TMC6200. Must be called before motor
 * will function properly. Returns true if motor init was successful.
 */
uint8_t MotorInit();

uint8_t MotorInitEncoder();

#endif /* USERDRIVERS_SPIMOTORDRIVER_H_ */
