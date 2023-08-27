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

#include "MotorParameters.h"

extern SPI_HandleTypeDef hspi1;

#define TIMEOUT 50

static velocity_t targetVelocity;

static ramp_point_t motorLowSpeedRamp[] = {
    {-1,500},
    {100, 80},
    {300, 70},
    {1200, 60},
    {2400,60},
    {3000, 30}
};
/**
 * Config and state of the low speed motor
 */
static motor_t motorLowSpeed = {
    MOTOR_LOW_SPEED,
    0,
    0,
    motorLowSpeedRamp,
    6
};

static ramp_point_t motorHighSpeedRamp[] = {
    {-1,500},
    {100, 80},
    {300, 70},
    {1200, 60},
    {2400,60},
    {3000, 30}
};
/**
 * Config and state of the high speed motor
 */
static motor_t motorHighSpeed = {
    MOTOR_HIGH_SPEED,
    0,
    0,
    motorHighSpeedRamp,
    6
};

/**
 * Pointer to the motor that is actively being used.
 * Use this when you want to reference the config and state of the active motor.
 */
static motor_t* activeMotor = &motorLowSpeed;


// => SPI wrapper
/**
 * @brief Perform SPI read and write operation for TMC4671 chip.
 * @param motor Motor ID.
 * @param data Data byte to be sent.
 * @param lastTransfer Flag indicating if this is the last transfer in the sequence.
 * @return Received data byte.
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

/**
 * @brief Wrapper function for TMC6200 read and write using tmc4671_readwriteByte.
 * @param motor Motor ID.
 * @param data Data byte to be sent.
 * @param lastTransfer Flag indicating if this is the last transfer in the sequence.
 * @return Received data byte.
 */
uint8_t tmc6200_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer)
{
	return tmc4671_readwriteByte(motor, data, lastTransfer);
}

// <= SPI wrapper

/**
 * @brief Set the Chip Select (CS) pin state for a specific motor.
 * @param cs Motor ID.
 * @param state GPIO pin state (GPIO_PIN_SET or GPIO_PIN_RESET).
 */
void MotorSetCS(uint8_t cs, GPIO_PinState state)
{
	switch (cs) {
	case TMC4671_CS:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, state);
		break;
	case TMC6200_CS:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, state);
		break;
        default:
                break;
	}
}

/**
 * @brief Select a motor by setting the activeMotor pointer and related configurations.
 * Disables the motor driver while switching motors.
 * @param code MotorCode enum indicating the selected motor.
 */
void MotorSelect(MotorCode code) {
        uint8_t didChange = activeMotor->id != code;

        if (didChange) {
                MotorEnableDriver(RESET);
                osDelayUntil(osKernelGetTickCount() + 100);
        }

        activeMotor = code == MOTOR_LOW_SPEED ? &motorLowSpeed : &motorHighSpeed;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, code == MOTOR_LOW_SPEED ? GPIO_PIN_SET : GPIO_PIN_RESET);

        if (didChange) {
                MotorInit();
        }
}

/**
 * @brief Initialize the motor and associated peripherals.
 * @return 1 on success, 0 on failure.
 */
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

	MotorEnableDriver(Set);
	HAL_Delay(100);

#ifdef MOTOR_CLEAR_CHARGE_PUMP_FAULT
	MotorClearChargePump();
	HAL_Delay(100);
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
	uint32_t shortConf = tmc6200_readInt(TMC6200_CS, TMC6200_SHORT_CONF);

	DebugPrint("Read [%08x]", nPolePairs);
	DebugPrint("Read [%08x]", shortConf);

	if (shortConf == MOTOR_CONFIG_DRIVER_SHORT_CONFIG)
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

        MotorSelect(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_SET ? MOTOR_LOW_SPEED : MOTOR_HIGH_SPEED);

        // AKA BANG BANG
	#ifdef ABN
        #ifdef MOTOR_CONFIG_AUTO_INIT_ENCODER
		if (MotorInitEncoder() != 0) {
			DebugPrint("Failed to init encoder");
			return false;
		}
        #endif
        #endif

	DebugPrint("Motor Initialized!");

	SystemSetMotorInitializing(Clear);

        // Turn off fault light to signal that init is done.
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	return true;
}

/**
 * @brief Basic function for validating connection to the TMC4671 and TMC6200.
 * @return 1 if the connection is valid, 0 otherwise.
 */
PUBLIC uint32_t MotorValidateSPI()
{
	return tmc4671_readInt(TMC4671_CS, TMC4671_MOTOR_TYPE_N_POLE_PAIRS) == MOTOR_CONFIG_N_POLE_PAIRS && tmc6200_readInt(TMC6200_CS, TMC6200_GCONF) == MOTOR_CONFIG_DRIVER_GENERAL_CONFIG;
}

// ===== Motor interaction =====

/**
 * @brief Rotate the motor at a specified velocity.
 * @param velocity Target velocity in RPM.
 * @return 0 on success.
 */
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

/**
 * @brief Initialize the encoder for the motor.
 * This will set the motor to open loop and rotate in reverse while initializing.
 * @return 0 on success, 1 on failure.
 */
PUBLIC uint8_t MotorInitEncoder() {

	uint8_t t = MOTOR_CONFIG_ABN_INIT_VELOCITY;
	
	// If not reversing, then reverse t because we want this to spin in reverse
	if (SystemGetReverseVelocity() == Clear) {
		t *= -1;
	}

        // Set acceleration to 60
        tmc4671_writeInt(TMC4671_CS, TMC4671_OPENLOOP_ACCELERATION, MOTOR_CONFIG_ABN_INIT_ACCELERATION);

        // Set velocity to reverse at 10 RPM
        tmc4671_writeInt(TMC4671_CS, TMC4671_OPENLOOP_VELOCITY_TARGET, t);

        tmc4671_writeInt(TMC4671_CS, TMC4671_UQ_UD_EXT, MOTOR_CONFIG_ABN_INIT_UQ_UD_EXIT);

        // Use Open Loop Mode (Phi E Selection)
        tmc4671_writeInt(TMC4671_CS, TMC4671_PHI_E_SELECTION, 2);

        // Use Motion Mode (UQ_UD_EXT)
        tmc4671_switchToMotionMode(TMC4671_CS, 8);

        HAL_Delay(300);

        // If the motor is not rotating, reverse direction.
        if (abs(MotorGetActualVelocity()) <= 2) {
                // Reverse motor
                tmc4671_writeInt(TMC4671_CS, TMC4671_OPENLOOP_VELOCITY_TARGET, t * -1);
                HAL_Delay(300);
        }

        if (abs(MotorGetActualVelocity()) < 2) {
                return 1;
        }

        // Stop the motor
        tmc4671_writeInt(TMC4671_CS, TMC4671_OPENLOOP_VELOCITY_TARGET, 0);

        int32_t openLoopPhiE = tmc4671_readInt(TMC4671_CS, TMC4671_PHI_E);

        // Need to clear both decoder counters

        // Raw decoder count, number of ticks counted
        tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_COUNT, 0);

        // Decoder count latched on N pulse.
        tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_COUNT_N, 0);

        // Shift the open loop angle to the most significant 16 bits of the number. The mechanical offset is set to 0.
        tmc4671_writeInt(TMC4671_CS, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, openLoopPhiE << 16);

        // Set ABN Encoder as Phi E Selection
        tmc4671_writeInt(TMC4671_CS, TMC4671_PHI_E_SELECTION, 3);

        return 0;
}

/**
 * @brief Perform periodic tasks related to the motor control.
 * Effectuate velocity ramping
 * @return 0 on success.
 */
PUBLIC uint8_t MotorPeriodicJob()
{
	DebugPrint("Writing Velocity Target: %d", TMC4671_PID_VELOCITY_TARGET);
        // Ramp logic
        velocity_t actualVelocity = MotorGetActualVelocity();

        // If motor needs negative velocities to advance, flip actual velocity
        if (SystemGetReverseVelocity() == Set) {
                actualVelocity *= -1;
        }

        velocity_t actualTargetVelocity;

        uint8_t ix = 0;
        while (ix < activeMotor->rampSize - 1 && actualVelocity > activeMotor->rampPoints[ix].rpm_target) {
                ix++;
        }

        if (targetVelocity > actualVelocity && (targetVelocity - actualVelocity) > activeMotor->rampPoints[ix].acceleration){
                actualTargetVelocity = actualVelocity + activeMotor->rampPoints[ix].acceleration;
                 // Acceleration is written in rpms per tick (1 tick = 150 ms)
        } else {
                actualTargetVelocity = targetVelocity;
        }

        if (SystemGetReverseVelocity() == Set) {
                actualTargetVelocity *= -1;
        }

        tmc4671_writeInt(TMC4671_CS, TMC4671_PID_VELOCITY_TARGET, actualTargetVelocity);

	return 0;
}

PUBLIC velocity_t MotorGetActualVelocity()
{
	return tmc4671_getActualVelocity(TMC4671_CS);
}

PUBLIC uint8_t MotorEnableDriver(flag_status_t enabled)
{
        // 4671
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, enabled == Set ? GPIO_PIN_SET : GPIO_PIN_RESET);

	return 0;
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
