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

PRIVATE typedef struct {
	uint16_t  startVoltage;
	uint16_t  initWaitTime;
	uint16_t  actualInitWaitTime;
	uint8_t   initState;
	uint8_t   initMode;
	uint16_t  torqueMeasurementFactor;  // uint8_t.uint8_t
	uint32_t  maximumCurrent;
	uint8_t	  motionMode;
	int32_t   actualVelocityPT1;
	int64_t	  akkuActualVelocity;
	int16_t   actualTorquePT1;
	int64_t   akkuActualTorque;
	int16_t   actualFluxPT1;
	int64_t   akkuActualFlux;
	int32_t   positionScaler;
	int32_t   linearScaler;
	int16_t   hall_phi_e_old;
	int16_t   hall_phi_e_new;
	int16_t   hall_actual_coarse_offset;
	uint16_t  last_Phi_E_Selection;
	uint32_t  last_UQ_UD_EXT;
	int16_t   last_PHI_E_EXT;
	uint8_t	  enableVelocityFeedForward;
} MotorDriverConfig_t;

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
PUBLIC uint32_t MotorRotate(int32_t velocity);

PUBLIC uint32_t MotorRotateTorque(int32_t torque);

PUBLIC uint32_t MotorPeriodicJob(uint32_t actualSystick);

PUBLIC uint32_t MotorEnableDriver(Enable_t enabled);

PUBLIC velocity_t MotorGetActualVelocity();
/**
 * Initialize registers in the TMC4671 and TMC6200. Must be called before motor
 * will function properly. Returns true if motor init was successful.
 */
PUBLIC uint32_t MotorInit();
PUBLIC uint32_t MotorDeInit();

PUBLIC void MotorHealth();

PUBLIC void MotorClearChargePump();

PUBLIC void MotorPrintFaults();


#endif /* USERDRIVERS_SPIMOTORDRIVER_H_ */
