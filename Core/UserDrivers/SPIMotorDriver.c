/*
 * SPIDriver.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#include <SPIMotorDriver.h>

#include "DatastoreModule.h"

#include "SerialDebugDriver.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

extern SPI_HandleTypeDef hspi1;

const uint32_t timeout = 50;


// => SPI wrapper
/**
 * Return the result of writing a single byte.
 */
uint8_t tmc4671_readwriteByte(const uint8_t motor, uint8_t data, uint8_t lastTransfer)
{
	// Set CS to low to signal start of data transfer
	setCS(motor, GPIO_PIN_RESET);

	HAL_StatusTypeDef status;

	// Pointer to receive buffer. (Can be interpreted as an array with a single byte.
	uint8_t rx_data[1];

	// Size == 1 because we will receive a single uint8_t AKA a single byte
	status = HAL_SPI_TransmitReceive(&hspi1, &data, rx_data, 1, timeout);

	// TODO: Error handle
	// Create datastore, store status of various tasks/systems
	if (status != HAL_OK) {

		datastoreSetSPIError(Set);

		switch (status) {
		case HAL_ERROR:
			DebugPrint("SPI Error to " + motor);
			break;
		case HAL_BUSY:
			DebugPrint("SPI Busy to " + motor);
		case HAL_TIMEOUT:
			DebugPrint("SPI Timeout to " + motor);
		case HAL_OK:
			DebugPrint("SPI Ok to " + motor);
		}
	}

	// If end of data transfer, set CS to high
	if (lastTransfer) {
		setCS(motor, GPIO_PIN_SET);
	}

	return *rx_data;
}
// <= SPI wrapper

void setCS(uint8_t cs, GPIO_PinState state) {
	switch (cs) {
		case TMC4671_CS:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, state);
			break;
		case TMC6200_CS:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, state);
			break;
		case TMC6200_EEPROM_1_CS:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, state);
			break;
		case TMC6200_EEPROM_2_CS:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, state);
			break;
	}
}

bool initMotor() {

	// Set all chip select lines to high
	setCS(TMC4671_CS, GPIO_PIN_SET);
	setCS(TMC6200_CS, GPIO_PIN_SET);
	setCS(TMC6200_EEPROM_1_CS, GPIO_PIN_SET);
	setCS(TMC6200_EEPROM_2_CS, GPIO_PIN_SET);


	// Motor type &  PWM configuration
	tmc4671_writeInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, MOTOR_CONFIG_N_POLE_PAIRS);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_POLARITIES, MOTOR_CONFIG_PWM_POLARITIES);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_MAXCNT, MOTOR_CONFIG_PWM_MAXCNT);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_BBM_H_BBM_L, MOTOR_CONFIG_PWM_BBM_H_BBM_L);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_SV_CHOP, MOTOR_CONFIG_PWM_SV_CHOP);

	// ADC configuration
	tmc4671_writeInt(TMC4671_CS, TMC4671_ADC_I_SELECT, MOTOR_CONFIG_ADC_I_SELECT);
	tmc4671_writeInt(TMC4671_CS, TMC4671_dsADC_MCFG_B_MCFG_A, MOTOR_CONFIG_dsADC_MCFG_B_MCFG_A);
	tmc4671_writeInt(TMC4671_CS, TMC4671_dsADC_MCLK_A, MOTOR_CONFIG_dsADC_MCLK_A);
	tmc4671_writeInt(TMC4671_CS, TMC4671_dsADC_MCLK_B, MOTOR_CONFIG_dsADC_MCLK_B);
	tmc4671_writeInt(TMC4671_CS, TMC4671_dsADC_MDEC_B_MDEC_A, MOTOR_CONFIG_dsADC_MDEC_B_MDEC_A);
	tmc4671_writeInt(TMC4671_CS, TMC4671_ADC_I0_SCALE_OFFSET, MOTOR_CONFIG_ADC_I0_SCALE_OFFSET);
	tmc4671_writeInt(TMC4671_CS, TMC4671_ADC_I1_SCALE_OFFSET, MOTOR_CONFIG_ADC_I1_SCALE_OFFSET);

	// Digital hall settings
	tmc4671_writeInt(TMC4671_CS, TMC4671_HALL_MODE, MOTOR_CONFIG_HALL_MODE);
	tmc4671_writeInt(TMC4671_CS, TMC4671_HALL_PHI_E_PHI_M_OFFSET, MOTOR_CONFIG_HALL_PHI_E_PHI_M_OFFSET);

	// Feedback selection
	tmc4671_writeInt(TMC4671_CS, TMC4671_PHI_E_SELECTION, MOTOR_CONFIG_PHI_E_SELECTION);
	tmc4671_writeInt(TMC4671_CS, TMC4671_VELOCITY_SELECTION, MOTOR_CONFIG_VELOCITY_SELECTION);

	// Limits
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_TORQUE_FLUX_LIMITS, MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS);

	// PI settings
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_TORQUE_P_TORQUE_I, MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_FLUX_P_FLUX_I, MOTOR_CONFIG_PID_FLUX_P_FLUX_I);

	// ===== Digital hall test drive =====

	// Switch to torque mode
	tmc4671_writeInt(TMC4671_CS, TMC4671_MODE_RAMP_MODE_MOTION, MOTOR_CONFIG_MODE_RAMP_MODE_MOTION);

	// ===== Set 6200 registers =====
//	tmc4671_writeInt(TMC6200_CS, 0x00, 0x00000000);
//	tmc4671_writeInt(TMC6200_CS, 0x01, 0x00000001);
//	tmc4671_writeInt(TMC6200_CS, 0x06, 0x00000000);
//	tmc4671_writeInt(TMC6200_CS, 0x08, 0x0000000E);
//	tmc4671_writeInt(TMC6200_CS, 0x09, 0x13010606);
//	tmc4671_writeInt(TMC6200_CS, 0x0A, 0x00080004);

	// ===== Verify registers were set =====

	// Read number of pole pairs
	uint32_t nPolePairs = tmc4671_readInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS);

	// If value is read is correct, than motor registers were properly set
	if (nPolePairs & MOTOR_CONFIG_N_POLE_PAIRS) {
		DebugPrint("Motor successfuly initialized!");
		return true;
	} else {
		DebugPrint("Failed to initialize motor!");
		return false;
	}
}

void writeTargetTorque(uint32_t torque) {
	DebugPrint("Writing target torque: %x [%i]", torque, torque);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_TORQUE_FLUX_TARGET, torque);
}

bool validateSPI() {
	return tmc4671_readInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS) & MOTOR_CONFIG_N_POLE_PAIRS;
}
