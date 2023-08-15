/*
 * SPIDriver.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#include "DataAggregationModule.h"
#include "SPIMotorDriver.h"

// TMC headers
#include "tmc/ic/TMC4671/TMC4671.h"
#include "tmc/ic/TMC6200/TMC6200.h"
#include "tmc/ic/TMC6200/TMC6200_Register.h"
#include "tmc/ramp/LinearRamp.h"

#include "MotorParameters.h"

extern SPI_HandleTypeDef hspi1;

#define TIMEOUT 50

static velocity_t targetVelocity;

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
	status = HAL_SPI_TransmitReceive(&hspi1, &data, rx_data, 1, TIMEOUT);

	if (status != HAL_OK)
	{

		SystemSetSPIError(Set);

		switch (status)
		{
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
	if (lastTransfer)
	{
		MotorSetCS(motor, GPIO_PIN_SET);
	}

	return *rx_data;
}

uint8_t tmc6200_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer)
{
	return tmc4671_readwriteByte(motor, data, lastTransfer);
}

// <= SPI wrapper

void MotorSetCS(uint8_t cs, GPIO_PinState state)
{
	switch (cs)
	{
	case TMC4671_CS:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, state);
		break;
	case TMC6200_CS:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, state);
		break;
	}
}

PUBLIC uint8_t MotorInit()
{
        // Enable Software Fault Light to signal init
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

	// Set all chip select lines to high
	MotorSetCS(TMC4671_CS, GPIO_PIN_SET);
	MotorSetCS(TMC6200_CS, GPIO_PIN_SET);

	SystemSetSPIError(Clear);
	SystemSetMotorInitializing(Set);

        // Stop the motor.
	tmc4671_writeInt(TMC4671_CS, TMC4671_MODE_RAMP_MODE_MOTION, 0);

	MotorEnableDriver(ENABLED);
	HAL_Delay(500);

#ifdef MOTOR_CLEAR_CHARGE_PUMP_FAULT
	MotorClearChargePump();
	HAL_Delay(250);
#endif

#if MOTOR_MODE == 0 || MOTOR_MODE == 1
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
	tmc4671_setTorqueFluxLimit_mA(TMC4671_CS, MOTOR_CONFIG_TORQUE_MESUREMENT_FACTOR, MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PIDOUT_UQ_UD_LIMITS, MOTOR_CONFIG_PID_UQ_UD_LIMITS);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_LIMIT, MAX_VELOCITY);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_ACCELERATION_LIMIT, MOTOR_CONFIG_ACCELERATION_LIMIT);

	// PI settings
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_TORQUE_P_TORQUE_I, MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_FLUX_P_FLUX_I, MOTOR_CONFIG_PID_FLUX_P_FLUX_I);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_P_VELOCITY_I, MOTOR_CONFIG_PID_VELOCITY_P_VELOCITY_I);

	// Stop motor
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_TARGET, 0);
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_TORQUE_FLUX_TARGET, 0);

	// Turn off feed forward
	tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_OFFSET, 0);

	// ===== Set 6200 registers =====
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

	if ((generalConf == MOTOR_CONFIG_DRIVER_GENERAL_CONFIG) && (shortConf == MOTOR_CONFIG_DRIVER_SHORT_CONFIG) && (driveConf == MOTOR_CONFIG_DRIVER_DRIVE_CONFIG))
	{
		DebugPrint("Motor Driver [" MOTOR_DRIVER_LABEL "] successfuly initialized!");
	}
	else
	{
		DebugPrint("Failed to initialize Motor Driver [" MOTOR_DRIVER_LABEL "]");
		return false;
	}

	// If value is read is correct, than motor registers were properly set
	if (nPolePairs == MOTOR_CONFIG_N_POLE_PAIRS)
	{
		DebugPrint("Motor Controller [" MOTOR_LABEL "] successfuly initialized!");
	}
	else
	{
		DebugPrint("Failed to initialize Motor Controller [" MOTOR_LABEL "]");
		return false;
	}

#elif MOTOR_MODE == 2
	MotorSetCS(TMC4671_CS, GPIO_PIN_SET);
	MotorSetCS(TMC6200_CS, GPIO_PIN_SET);
	MotorEnableDriver(ENABLED);
	HAL_Delay(1000);

#ifdef MOTOR_CLEAR_CHARGE_PUMP_FAULT
	MotorClearChargePump();
#endif
#endif

        // AKA BANG BANG
	#ifdef MOTOR_CONFIG_AUTO_INIT_ENCODER
		if (MotorInitEncoder() != 0) {
			DebugPrint("Failed to init encoder");
			return false;
		}
	#endif

	DebugPrint("Motor Initialized!");

	SystemSetMotorInitializing(Clear);

        // Turn off fault light to signal that init is done.
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	return true;
}

/**
 * Basic function for validation connection to the TMC4671
 */
PUBLIC uint32_t MotorValidateSPI()
{
	return tmc4671_readInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS) == MOTOR_CONFIG_N_POLE_PAIRS && tmc6200_readInt(TMC6200_CS, TMC6200_GCONF) == MOTOR_CONFIG_DRIVER_GENERAL_CONFIG;
}

// ===== Motor interaction =====

PUBLIC uint8_t MotorRotateVelocity(velocity_t velocity)
{
        // If the motor is not turning
	if (velocity == 0) {
                // Stop the motor from attempting to spin. By default, the motor will try to keep it's current position.
		tmc4671_switchToMotionMode(TMC4671_CS, 0);
		targetVelocity = 0;
	}
	else {
		tmc4671_switchToMotionMode(TMC4671_CS, TMC4671_MOTION_MODE_VELOCITY);
		targetVelocity = velocity;
	}

	return 0;
}

PRIVATE int16_t MotorGetS16CircleDifference(int16_t newValue, int16_t oldValue)
{
	return (newValue - oldValue);
}

PRIVATE static void MotorInitEncoderPeriodicJob(uint8_t motor, uint8_t *initState, uint16_t *actualInitWaitTime,
		int16_t *hall_phi_e_old, int16_t *hall_phi_e_new, int16_t *hall_actual_coarse_offset)
{
#define STATE_NOTHING_TO_DO 0
#define STATE_START_INIT 1
#define STATE_WAIT_INIT_TIME 2

	switch (*initState)
	{
	case STATE_NOTHING_TO_DO:
		*actualInitWaitTime = 0;
		break;
	case STATE_START_INIT: // started by writing 1 to initState
		// set ABN_DECODER_PHI_E_OFFSET to zero
		tmc4671_writeRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, 0);

		// read actual hall angle
		*hall_phi_e_old = TMC4671_FIELD_READ(motor, TMC4671_HALL_PHI_E_INTERPOLATED_PHI_E, TMC4671_HALL_PHI_E_MASK, TMC4671_HALL_PHI_E_SHIFT);

		// read actual abn_decoder angle and compute difference to actual hall angle
		*hall_actual_coarse_offset = MotorGetS16CircleDifference(*hall_phi_e_old, (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M, BIT_16_TO_31));

		// set ABN_DECODER_PHI_E_OFFSET to actual hall-abn-difference, to use the actual hall angle for coarse initialization
		tmc4671_writeRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, *hall_actual_coarse_offset);

		*initState = STATE_WAIT_INIT_TIME;
		break;
	case STATE_WAIT_INIT_TIME:
		// read actual hall angle
		*hall_phi_e_new = TMC4671_FIELD_READ(motor, TMC4671_HALL_PHI_E_INTERPOLATED_PHI_E, TMC4671_HALL_PHI_E_MASK, TMC4671_HALL_PHI_E_SHIFT);

		// wait until hall angle changed
		if(*hall_phi_e_old != *hall_phi_e_new)
		{
			// estimated value = old value + diff between old and new (handle int16_t overrun)
			int16_t hall_phi_e_estimated = *hall_phi_e_old + MotorGetS16CircleDifference(*hall_phi_e_new, *hall_phi_e_old)/2;

			// read actual abn_decoder angle and consider last set abn_decoder_offset
			int16_t abn_phi_e_actual = (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M, BIT_16_TO_31) - *hall_actual_coarse_offset;

			// set ABN_DECODER_PHI_E_OFFSET to actual estimated angle - abn_phi_e_actual difference
			tmc4671_writeRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, MotorGetS16CircleDifference(hall_phi_e_estimated, abn_phi_e_actual));
			
			// go to ready state
			*initState = 0;
		}
		break;
	default:
		*initState = 0;
		break;
	}
}

PUBLIC uint8_t MotorInitEncoder() {

	torque_t t = MOTOR_CONFIG_ENCODER_INIT_STRENGTH;
	
	// If not reversing, then reverse t because we want this to spin in reverse
	if (SystemGetReverseVelocity() == Clear) {
		t *= -1;
	}

#define HALL 5
#define ENC 3

	tmc4671_writeRegister16BitValue(TMC4671_CS, TMC4671_PHI_E_SELECTION, BIT_0_TO_15, HALL);

	// Start spining the motor	
	tmc4671_switchToMotionMode(TMC4671_CS, TMC4671_MOTION_MODE_TORQUE);
	tmc4671_setTargetTorque_raw(TMC4671_CS, t);

	HAL_Delay(500);

	// Check if motor actually started spinning
	if ((SystemGetReverseVelocity() == Set && MotorGetActualVelocity() < MOTOR_CONFIG_ENCODER_INIT_SPEED) || (SystemGetReverseVelocity() == Clear && MotorGetActualVelocity() > -1 * MOTOR_CONFIG_ENCODER_INIT_SPEED)) {
		tmc4671_setTargetTorque_raw(TMC4671_CS, 0);
		return 1;
	}

	uint8_t initState = 1;
	uint16_t actualInitWaitTime;
	int16_t hall_phi_e_old;
	int16_t hall_phi_e_new;
	int16_t hall_actual_coarse_offset;

	uint8_t c = 0;

	MotorInitEncoderPeriodicJob(TMC4671_CS, &initState, &actualInitWaitTime, &hall_phi_e_old, &hall_phi_e_new, &hall_actual_coarse_offset);

	while (initState != 0 && c <= 4) {
		HAL_Delay(300);
		
		MotorInitEncoderPeriodicJob(TMC4671_CS, &initState, &actualInitWaitTime, &hall_phi_e_old, &hall_phi_e_new, &hall_actual_coarse_offset);
		c++;
	}

	// Failed to init encoder
	if (initState != 0) {
		return 1;
	}

	// Stop motor
	tmc4671_setTargetTorque_raw(TMC4671_CS, 0);
	HAL_Delay(200);

	// switch to encoder
	tmc4671_writeRegister16BitValue(TMC4671_CS, TMC4671_PHI_E_SELECTION, BIT_0_TO_15, ENC);
	tmc4671_setTargetTorque_raw(TMC4671_CS, t);
	HAL_Delay(600);

	if ((SystemGetReverseVelocity() == Set && MotorGetActualVelocity() < MOTOR_CONFIG_ENCODER_INIT_SPEED) || (SystemGetReverseVelocity() == Clear && MotorGetActualVelocity() > -1 * MOTOR_CONFIG_ENCODER_INIT_SPEED)) {
		tmc4671_setTargetTorque_raw(TMC4671_CS, 0);
		return 1;
	}

	tmc4671_setTargetTorque_raw(TMC4671_CS, 0);
	return 0;
}

#define RAMP_SIZE 4
static ramp_point_t ramp[RAMP_SIZE] = {
    {-1,500},
    {600,25},
    {2400,50},
    {3000,25}
};

PUBLIC uint8_t MotorPeriodicJob()
{
	DebugPrint("Writing Velocity Target: %d", TMC4671_PID_VELOCITY_TARGET);
        // Ramp logic
        velocity_t velocity_rpm = MotorGetActualVelocity();
        uint8_t ix;
        velocity_t actual_target_velocity;
        while (ix < RAMP_SIZE - 1 && velocity_rpm > ramp[ix].rpm_target) {
                ix++;
        }

        if (targetVelocity - velocity_rpm > ramp[ix]){
                 actual_target_velocity = velocity_rpm + ramp[ix].acceleration
                 // Acceleration is written in rpms per tick (1 tick = 150 ms)
        } else {
                 actual_target_velocity = targetVelocity
        }

        tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_TARGET, actual_target_velocity);

	return 0;
}

PUBLIC velocity_t MotorGetActualVelocity()
{
	return tmc4671_getActualVelocity(TMC4671_CS);
}

PUBLIC uint8_t MotorEnableDriver(Enable_t enabled)
{
        // 4671
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, enabled == ENABLED ? GPIO_PIN_SET : GPIO_PIN_RESET);

	return 0;
}

PUBLIC uint8_t MotorDeInit()
{
	return MotorEnableDriver(DISABLED);
}

PUBLIC void MotorHealth()
{
	DebugPrint("MotorHealth: NOT IMPLEMENTED");
}

PUBLIC void MotorClearChargePump()
{
	// Clear the charge pump fault generated by our custom PCB
	tmc6200_writeInt(TMC6200_CS, TMC6200_GSTAT, 0x00000008);
	DebugPrint("Cleared Charge Pump Fault");
}

PUBLIC void MotorPrintFaults()
{
	uint32_t stats = tmc6200_readInt(TMC6200_CS, TMC6200_GSTAT);
        if (stats != 0 && stats != 1) {
                DebugPrint("FAULTS: %04x", stats);
        }
}
