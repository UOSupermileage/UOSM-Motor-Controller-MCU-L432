/*
 * SPIDriver.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#include "DataAggregationModule.h"
#include "SPIMotorDriver.h"

#include "SerialDebugDriver.h"

// TMC headers
#include "tmc/ic/TMC4671/TMC4671.h"
#include "tmc/ic/TMC6200/TMC6200.h"
#include "tmc/ic/TMC6200/TMC6200_Register.h"
#include "tmc/ramp/LinearRamp.h"

#include "MotorParameters.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

extern SPI_HandleTypeDef hspi1;

const uint32_t timeout = 50;

MotorDriverConfig_t motorDriverConfig;

// variables for ramp generator support
TMC_LinearRamp rampGenerator;
int32_t lastRampTargetVelocity;
int32_t lastRampTargetPosition;
uint8_t actualMotionMode;

uint32_t targetTorqueForTorqueMode = 0;
uint32_t torqueAcceleration = MOTOR_TORQUE_ACCELERATION;

// => SPI wrapper
/**
 * Return the result of writing a single byte.
 */
uint8_t tmc4671_readwriteByte(const uint8_t motor, uint8_t data, uint8_t lastTransfer)
{
	// Set CS to low to signal start of data transfer
	MotorSetCS(motor, GPIO_PIN_RESET);

	HAL_StatusTypeDef status;

	// Pointer to receive buffer. (Can be interpreted as an array with a single byte.
	uint8_t rx_data[1];

	// Size == 1 because we will receive a single uint8_t AKA a single byte
	status = HAL_SPI_TransmitReceive(&hspi1, &data, rx_data, 1, timeout);

	if (status != HAL_OK) {

		SystemSetSPIError(Set);

		switch (status) {
		case HAL_ERROR:
			DebugPrint("SPI Error to %d", motor);
			break;
		case HAL_BUSY:
			DebugPrint("SPI Busy to %d", motor);
			break;
		case HAL_TIMEOUT:
			DebugPrint("SPI Timeout to %d", motor);
			break;
		case HAL_OK:
			DebugPrint("SPI Ok to %d", motor);
			break;
		}
	}

	// If end of data transfer, set CS to high
	if (lastTransfer) {
		MotorSetCS(motor, GPIO_PIN_SET);
	}

	return *rx_data;
}

uint8_t tmc6200_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer)
{
	return tmc4671_readwriteByte(motor, data, lastTransfer);
}

// <= SPI wrapper

void MotorSetCS(uint8_t cs, GPIO_PinState state) {
	switch (cs) {
		case TMC4671_CS:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, state);
			break;
		case TMC6200_CS:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, state);
			break;
	}
}

PUBLIC uint32_t MotorInit() {

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);


	// Set all chip select lines to high
	MotorSetCS(TMC4671_CS, GPIO_PIN_SET);
	MotorSetCS(TMC6200_CS, GPIO_PIN_SET);

	SystemSetSPIError(Clear);

	MotorEnableDriver(ENABLED);
	HAL_Delay(250);

#ifdef MOTOR_CLEAR_CHARGE_PUMP_FAULT
	MotorClearChargePump();
	HAL_Delay(250);
#endif

#if MOTOR_MODE == 0 || MOTOR_MODE == 1

	motorDriverConfig.initWaitTime = 1000;
	motorDriverConfig.startVoltage             	= 6000;
	motorDriverConfig.initMode                 	=  MOTOR_INIT_MODE;
	motorDriverConfig.initState					= 0;
	motorDriverConfig.hall_phi_e_old			= 0;
	motorDriverConfig.hall_phi_e_new			= 0;
	motorDriverConfig.hall_actual_coarse_offset	= 0;
	motorDriverConfig.last_Phi_E_Selection		= 5;
	motorDriverConfig.last_UQ_UD_EXT			= 0;
	motorDriverConfig.last_PHI_E_EXT			= 0;
	motorDriverConfig.torqueMeasurementFactor  	= MOTOR_CONFIG_TORQUE_MESUREMENT_FACTOR;
	motorDriverConfig.maximumCurrent			= MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS;
	motorDriverConfig.actualVelocityPT1			= 0;
	motorDriverConfig.akkuActualVelocity       	= 0;
	motorDriverConfig.actualTorquePT1			= 0;
	motorDriverConfig.akkuActualTorque         	= 0;
	motorDriverConfig.positionScaler			= POSITION_SCALE_MAX;
	motorDriverConfig.enableVelocityFeedForward	= true;
	motorDriverConfig.linearScaler             	= 30000; // µm / rotation

	// Motor type &  PWM configuration
	tmc4671_writeInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, MOTOR_CONFIG_N_POLE_PAIRS);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_MAXCNT, MOTOR_CONFIG_PWM_MAXCNT);

	// Set default polarity for power stage on init
	tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_POLARITIES, MOTOR_CONFIG_PWM_POLARITIES);
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

#ifdef ABN
	tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_MODE, MOTOR_CONFIG_ABN_DECODER_MODE);
	tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_PPR, MOTOR_CONFIG_ABN_DECODER_PPR);
	tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_COUNT, MOTOR_CONFIG_ABN_DECODER_COUNT);
	tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_COUNT_N, MOTOR_CONFIG_ABN_DECODER_COUNT_N);
	tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, MOTOR_CONFIG_ABN_DECODER_PHI_E_PHI_M_OFFSET);
#endif

	// Digital hall settings
	tmc4671_writeInt(TMC4671_CS, TMC4671_HALL_MODE, MOTOR_CONFIG_HALL_MODE);
	tmc4671_writeInt(TMC4671_CS, TMC4671_HALL_PHI_E_PHI_M_OFFSET, MOTOR_CONFIG_HALL_PHI_E_PHI_M_OFFSET);

	// Feedback selection
	tmc4671_writeInt(TMC4671_CS, TMC4671_PHI_E_SELECTION, MOTOR_CONFIG_STARTING_PHI_E_SELECTION);
	tmc4671_writeInt(TMC4671_CS, TMC4671_VELOCITY_SELECTION, MOTOR_CONFIG_VELOCITY_SELECTION);
	tmc4671_writeInt(TMC4671_CS, TMC4671_MODE_RAMP_MODE_MOTION, MOTOR_CONFIG_MODE_RAMP_MODE_MOTION);

	// Limits
	tmc4671_setTorqueFluxLimit_mA(TMC4671_CS, motorDriverConfig.torqueMeasurementFactor, MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PIDOUT_UQ_UD_LIMITS, MOTOR_CONFIG_PID_UQ_UD_LIMITS);

	// PI settings
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_TORQUE_P_TORQUE_I, MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_FLUX_P_FLUX_I, MOTOR_CONFIG_PID_FLUX_P_FLUX_I);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_P_VELOCITY_I, MOTOR_CONFIG_PID_VELOCITY_P_VELOCITY_I);

	// Stop motor
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_TARGET, 0);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_TORQUE_FLUX_TARGET, 0);

	// ===== Init Ramp Generator =====
	tmc_linearRamp_init(&rampGenerator);
	actualMotionMode = TMC4671_MOTION_MODE_STOPPED;
	lastRampTargetVelocity = 0;
	lastRampTargetPosition = 0;

	rampGenerator.maxVelocity = (uint32_t)tmc4671_readInt(TMC4671_CS, TMC4671_PID_VELOCITY_LIMIT);
	rampGenerator.acceleration = (uint32_t)tmc4671_readInt(TMC4671_CS, TMC4671_PID_ACCELERATION_LIMIT);


//	 ===== Set 6200 registers =====
	tmc6200_writeInt(TMC6200_CS, TMC6200_GCONF, MOTOR_CONFIG_DRIVER_GENERAL_CONFIG);
	tmc6200_writeInt(TMC6200_CS, TMC6200_SHORT_CONF, MOTOR_CONFIG_DRIVER_SHORT_CONFIG);
	tmc6200_writeInt(TMC6200_CS, TMC6200_DRV_CONF, MOTOR_CONFIG_DRIVER_DRIVE_CONFIG);

	// ===== Verify registers were set =====

	// Read TMC4671 values for validation
	uint32_t nPolePairs = tmc4671_readInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS);

	// Read TMC6200 values for validation.
	uint32_t generalConf = tmc6200_readInt(TMC6200_CS, TMC6200_GCONF);
	uint32_t shortConf = tmc6200_readInt(TMC6200_CS, TMC6200_SHORT_CONF);
	uint32_t driveConf = tmc6200_readInt(TMC6200_CS, TMC6200_DRV_CONF);

	DebugPrint("Read [%08x]", nPolePairs);
	DebugPrint("Read [%08x], [%08x], [%08x]", generalConf, shortConf, driveConf);


	if ((generalConf == MOTOR_CONFIG_DRIVER_GENERAL_CONFIG) && (shortConf == MOTOR_CONFIG_DRIVER_SHORT_CONFIG) && (driveConf == MOTOR_CONFIG_DRIVER_DRIVE_CONFIG)) {
		DebugPrint("Motor Driver [" MOTOR_DRIVER_LABEL "] successfuly initialized!");
	} else {
		DebugPrint("Failed to initialize Motor Driver [" MOTOR_DRIVER_LABEL "]");
		return false;
	}

	// If value is read is correct, than motor registers were properly set
	if (nPolePairs == MOTOR_CONFIG_N_POLE_PAIRS) {
		DebugPrint("Motor Controller [" MOTOR_LABEL "] successfuly initialized!");
	}
	else {
		DebugPrint("Failed to initialize Motor Controller [" MOTOR_LABEL "]");
		return false;
	}
//
//	uint32_t stats1 = tmc6200_readInt(TMC6200_CS, TMC6200_GSTAT);
//	if (stats1 != 0 && stats1 != 1) {
//		DebugPrint("Motor Driver [" MOTOR_DRIVER_LABEL "] Fault Detected: [%x]", stats1);
//		MotorPrintFaults();
////		MotorEnableDriver(DISABLED);
//		return false;
//	}

#elif MOTOR_MODE == 2
    MotorSetCS(TMC4671_CS, GPIO_PIN_SET);
    MotorSetCS(TMC6200_CS, GPIO_PIN_SET);
    MotorEnableDriver(ENABLED);
    HAL_Delay(1000);

	#ifdef MOTOR_CLEAR_CHARGE_PUMP_FAULT
		MotorClearChargePump();
	#endif
#endif


	if (SystemGetSPIError() == Set) {
		DebugPrint("Failed to init do to SPI error");
		return false;
	}

	DebugPrint("Motor Initialized!");

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	return true;
}

/**
 * Basic function for validation connection to the TMC4671
 */
PUBLIC uint32_t MotorValidateSPI() {
	return tmc4671_readInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS) == MOTOR_CONFIG_N_POLE_PAIRS && tmc6200_readInt(TMC6200_CS, TMC6200_GCONF) == MOTOR_CONFIG_DRIVER_GENERAL_CONFIG;
}

// ===== Motor interaction =====


PUBLIC uint32_t MotorRotate(int32_t velocity) {
	tmc4671_switchToMotionMode(TMC4671_CS, TMC4671_MOTION_MODE_VELOCITY);
	actualMotionMode = TMC4671_MOTION_MODE_VELOCITY;
	rampGenerator.targetVelocity = velocity;
	return 0;
}

PUBLIC uint32_t MotorRotateTorque(int32_t torque) {

	DebugPrint("MotorRotateTorque: input %x", torque);

	tmc4671_switchToMotionMode(TMC4671_CS, TMC4671_MOTION_MODE_TORQUE);
	actualMotionMode = TMC4671_MOTION_MODE_TORQUE;

	uint32_t diff = abs(torque - targetTorqueForTorqueMode);

	if (diff > torqueAcceleration) {
		if (torque < targetTorqueForTorqueMode) {
			targetTorqueForTorqueMode -= torqueAcceleration;
		} else {
			targetTorqueForTorqueMode += torqueAcceleration;
		}
	} else {
		targetTorqueForTorqueMode = torque;
	}

	DebugPrint("MotorRotateTorque: output %d", torque);

	return 0;
}

PUBLIC uint32_t MotorPeriodicJob(uint32_t actualSystick) {

	// Motor is spinning above minimum RPM
	if (tmc4671_readInt(TMC4671_CS, TMC4671_PID_VELOCITY_ACTUAL) < MOTOR_CONFIG_MIN_VELOCITY_FOR_ENCODER_INIT) {
		// Do encoder init
		tmc4671_updatePhiSelectionAndInitialize(TMC4671_CS, tmc4671_readInt(TMC4671_CS, TMC4671_PHI_E_SELECTION), MOTOR_CONFIG_TARGET_PHI_E_SELECTION, motorDriverConfig.initMode,  &motorDriverConfig.initState);
	} else {
		DebugPrint("Motor hasn't reached target RPM yet...");
	}

	tmc4671_periodicJob(TMC4671_CS, actualSystick, motorDriverConfig.initMode, &motorDriverConfig.initState, motorDriverConfig.initWaitTime, &motorDriverConfig.actualInitWaitTime, motorDriverConfig.startVoltage, &motorDriverConfig.hall_phi_e_old, &motorDriverConfig.hall_phi_e_new, &motorDriverConfig.hall_actual_coarse_offset, &motorDriverConfig.last_Phi_E_Selection, &motorDriverConfig.last_UQ_UD_EXT, &motorDriverConfig.last_PHI_E_EXT);

	// 1ms velocity ramp handling
	static uint32_t lastSystick;

	if (lastSystick != actualSystick) {

		DebugPrint("SysTick");

		// filter actual velocity
		motorDriverConfig.actualVelocityPT1 = tmc_filterPT1(&motorDriverConfig.akkuActualVelocity, tmc4671_getActualVelocity(TMC4671_CS), motorDriverConfig.actualVelocityPT1, 3, 8);

		// filter actual current
		int16_t actualCurrentRaw = TMC4671_FIELD_READ(TMC4671_CS, TMC4671_PID_TORQUE_FLUX_ACTUAL, TMC4671_PID_TORQUE_ACTUAL_MASK, TMC4671_PID_TORQUE_ACTUAL_SHIFT);
		if ((actualCurrentRaw > -32000) && (actualCurrentRaw < 32000)) {
			int32_t actualCurrent = ((int32_t)actualCurrentRaw * (int32_t)motorDriverConfig.torqueMeasurementFactor) / 256;
			motorDriverConfig.actualTorquePT1 = tmc_filterPT1(&motorDriverConfig.akkuActualTorque , actualCurrent, motorDriverConfig.actualTorquePT1, 4, 8);
		}

		// filter actual flux
		int16_t actualFluxRaw = TMC4671_FIELD_READ(TMC4671_CS, TMC4671_PID_TORQUE_FLUX_ACTUAL, TMC4671_PID_FLUX_ACTUAL_MASK, TMC4671_PID_FLUX_ACTUAL_SHIFT);
		if ((actualFluxRaw > -32000) && (actualFluxRaw < 32000)) {
			int32_t actualFlux = ((int32_t)actualFluxRaw * (int32_t)motorDriverConfig.torqueMeasurementFactor) / 256;
			motorDriverConfig.actualFluxPT1 = tmc_filterPT1(&motorDriverConfig.akkuActualFlux , actualFlux, motorDriverConfig.actualFluxPT1, 2, 8);
		}

		DebugPrint("MotionMode: %x", actualMotionMode);

		if (actualMotionMode == TMC4671_MOTION_MODE_VELOCITY) {
			tmc_linearRamp_computeRampVelocity(&rampGenerator);

			// set new target velocity (only if changed)
			if (rampGenerator.rampVelocity != lastRampTargetVelocity) {
				// set new target velocity
				DebugPrint("Writing Velocity Target: %d", TMC4671_PID_VELOCITY_TARGET);
				tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_TARGET, rampGenerator.rampVelocity);
				lastRampTargetVelocity = rampGenerator.rampVelocity;

				// turn off velocity feed forward
				tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_OFFSET, 0);
			} else {
				DebugPrint("Ramp Velocity Unchanged! [%d]", lastRampTargetVelocity);
			}

			// keep position ramp and target position on track
			tmc4671_writeInt(TMC4671_CS, TMC4671_PID_POSITION_TARGET, tmc4671_readInt(TMC4671_CS, TMC4671_PID_POSITION_ACTUAL));
			rampGenerator.rampPosition = tmc4671_readInt(TMC4671_CS, TMC4671_PID_POSITION_ACTUAL);
			rampGenerator.lastdXRest = 0;
		} else if (actualMotionMode == TMC4671_MOTION_MODE_TORQUE) {
			DebugPrint("Actual Target Torque: %dmA", targetTorqueForTorqueMode);
			tmc4671_setTargetTorque_mA(TMC4671_CS, motorDriverConfig.torqueMeasurementFactor, -1 * targetTorqueForTorqueMode);

//			if (targetTorqueForTorqueMode > 800) {
//				tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_SV_CHOP, 0x00000007);
//			} else {
//				tmc4671_writeInt(TMC4671_CS, TMC4671_PWM_SV_CHOP, 0x00000000);
//			}

		} else {
			DebugPrint("Error. MotionMode [%d] is not implemented!", actualMotionMode);
		}

		lastSystick = actualSystick;
	}

	return 0;
}

PUBLIC velocity_t MotorGetActualVelocity() {
	return tmc4671_getActualVelocity(TMC4671_CS);
}

PUBLIC uint32_t MotorEnableDriver(Enable_t enabled) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, enabled == ENABLED ? GPIO_PIN_SET : GPIO_PIN_RESET);
	return 0;
}

PUBLIC uint32_t MotorDeInit() {
	return MotorEnableDriver(DISABLED);
}

PUBLIC void MotorHealth() {
	DebugPrint("MotorHealth: NOT IMPLEMENTED");
}

PUBLIC void MotorClearChargePump() {
//	// Clear the charge pump fault generated by our custom PCB
	tmc6200_writeInt(TMC6200_CS, TMC6200_GSTAT, 0x00000008);
	DebugPrint("Cleared Charge Pump Fault");
}

/**
 * NOT SURE IF THIS ACTUALLY WORKS!
 */
PUBLIC void MotorPrintFaults() {
	uint32_t stats = tmc6200_readInt(TMC6200_CS, TMC6200_GSTAT);

	if (stats & (1 << 1)) {
		DebugPrint("Fault: drv_otpw");
	}

	if (stats & (1 << 2)) {
		DebugPrint("Fault: drv_ot");
	}

	if (stats & (1 << 3)) {
		DebugPrint("Fault: uv_cp");
	}

	if (stats & (1 << 4)) {
		DebugPrint("Fault: shortdet_u");
	}

	if (stats & (1 << 5)) {
		DebugPrint("Fault: s2gu");
	}

	if (stats & (1 << 6)) {
		DebugPrint("Fault: s2vsu");
	}

	if (stats & (1 << 8)) {
		DebugPrint("Fault: shortdet_v");
	}

	if (stats & (1 << 9)) {
		DebugPrint("Fault: s2gv");
	}

	if (stats & (1 << 10)) {
		DebugPrint("Fault: s2vsv");
	}

	if (stats & (1 << 12)) {
		DebugPrint("Fault: shortdet_w");
	}

	if (stats & (1 << 13)) {
		DebugPrint("Fault: s2gw");
	}

	if (stats & (1 << 14)) {
		DebugPrint("Fault: s2vsw");
	}
}
