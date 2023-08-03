/*
 * MotorParameters.h
 *
 *	Defines parameters for motor.
 *	All motor specific settings should be placed here.
 *	Swapping motors should not change anything other than this file.
 *
 *  Created on: Feb. 28, 2023
 *      Author: jeremycote
 */

#ifndef INC_MOTORPARAMETERS_H_
#define INC_MOTORPARAMETERS_H_

#include "ApplicationTypes.h"

/**
 * Select Target Motor
 * 0 == QBL4208-81-04-019	(Tiny Motor) (Eval board)
 * 1 == BLK322D-48V-3000 	(Eval board)
 * 2 == BLK322D-48V-3000 	(Custom Board)
 *
 * Note: BLK322D-48V-3000 -> Motor in the car, 48V with a 3000 RPM max
 */
#define MOTOR 2

/**
 * Select Motor Mode
 * 0 == Normal operation, set registers in TMC chips, make motor spin
 * 1 == RTMI operation, set registers in TMC chips, do not make motor spin
 * 2 == Idle operation, do nothing except enable driver and clear charge pump if configured
 */

#define MOTOR_MODE 0

#define MOTOR_CONFIG_AUTO_INIT_ENCODER

#define MOTOR_CONFIG_ENCODER_INIT_STRENGTH 1500
#define MOTOR_CONFIG_ENCODER_INIT_SPEED 30

/*********************************************************************************
 *
 * 		Motion Mode Parameters
 * 		1 == Torque
 * 		2 == Velocity
 *      3 == Dynamometer / Position
 *
 **********************************************************************************/

#define MOTOR_CONFIG_MODE_RAMP_MODE_MOTION 2
/*********************************************************************************
 *
 * 		Fixed Throttle Value in RPM (Use for testing only)
 * 		When defined, override throttle input with this value.
 * 		Percentage of max RPM / max Torque depending on motion mode.
 *
 * 		Increments of 0.1%.
 * 		Ex: 200 -> 20%, 650 -> 65%
 *
 **********************************************************************************/

// #define MOTOR_FIXED_THROTTLE 1000

#define ABN_INIT_INTERVAL 3

/*********************************************************************************
 *
 * 		Dynamometer position offset
 *
 * 		Only applied when using positon motion mode.
 *
 **********************************************************************************/

#define MOTOR_CONFIG_POSITION_OFFSET_INCREMENT 100

/*********************************************************************************
 *
 * 		Motor Torque Acceleration Step Size
 * 		Defines the largest jump in torque allowed.
 * 		Larger jumps will be split into steps of this size.
 *
 * 		Only applied when using torque motion mode.
 *
 **********************************************************************************/

#define MOTOR_TORQUE_ACCELERATION 3000

/*********************************************************************************
 *
 * 		Serial Debug Paramters
 *
 **********************************************************************************/

#define MOTOR_LABEL "TMC4671"
#define MOTOR_DRIVER_LABEL "TMC6200"

/*********************************************************************************
 *
 * 		Motor Type
 *
 * 		0x0030004 -> BLDC motor with 4 pole pairs
 *
 **********************************************************************************/

#if MOTOR == 0
#define MOTOR_CONFIG_N_POLE_PAIRS (uint32_t)0x00030004
#elif MOTOR == 1
#define MOTOR_CONFIG_N_POLE_PAIRS (uint32_t)0x00030004
#elif MOTOR == 2
#define MOTOR_CONFIG_N_POLE_PAIRS (uint32_t)0x00030004
#endif

/*********************************************************************************
 *
 * 		PWM Parameters
 *
 **********************************************************************************/

#if MOTOR == 0
#define MOTOR_CONFIG_PWM_POLARITIES (uint32_t)0x00000000
#define MOTOR_CONFIG_PWM_MAXCNT (uint32_t)0x00000F9F

#define MOTOR_CONFIG_PWM_BBM_H_BBM_L (uint32_t)0x00001919
#define MOTOR_CONFIG_PWM_SV_CHOP (uint32_t)0x00000007
#elif MOTOR == 1
#define MOTOR_CONFIG_PWM_POLARITIES (uint32_t)0x00000000
#define MOTOR_CONFIG_PWM_MAXCNT (uint32_t)0x00000F9F

#define MOTOR_CONFIG_PWM_BBM_H_BBM_L (uint32_t)0x00001919
#define MOTOR_CONFIG_PWM_SV_CHOP (uint32_t)0x00000007
#elif MOTOR == 2
#define MOTOR_CONFIG_PWM_POLARITIES (uint32_t)0x00000000
#define MOTOR_CONFIG_PWM_MAXCNT (uint32_t)0x00000F9F

#define MOTOR_CONFIG_PWM_BBM_H_BBM_L (uint32_t)0x00001919
#define MOTOR_CONFIG_PWM_SV_CHOP (uint32_t)0x00000007
#endif

/*********************************************************************************
 *
 * 		ADC Parameters
 *
 **********************************************************************************/

#if MOTOR == 0
#define MOTOR_CONFIG_ADC_I_SELECT (uint32_t)0x24000100
#define MOTOR_CONFIG_dsADC_MCFG_B_MCFG_A (uint32_t)0x00100010
#define MOTOR_CONFIG_dsADC_MCLK_A (uint32_t)0x20000000
#define MOTOR_CONFIG_dsADC_MCLK_B (uint32_t)0x00000000
#define MOTOR_CONFIG_dsADC_MDEC_B_MDEC_A (uint32_t)0x014E014E
#define MOTOR_CONFIG_ADC_I0_SCALE_OFFSET (uint32_t)0xFF007FE3
#define MOTOR_CONFIG_ADC_I1_SCALE_OFFSET (uint32_t)0xFF0080E9
#elif MOTOR == 1
#define MOTOR_CONFIG_ADC_I_SELECT (uint32_t)0x24000100
#define MOTOR_CONFIG_dsADC_MCFG_B_MCFG_A (uint32_t)0x00100010
#define MOTOR_CONFIG_dsADC_MCLK_A (uint32_t)0x20000000
#define MOTOR_CONFIG_dsADC_MCLK_B (uint32_t)0x00000000
#define MOTOR_CONFIG_dsADC_MDEC_B_MDEC_A (uint32_t)0x014E014E
#define MOTOR_CONFIG_ADC_I0_SCALE_OFFSET (uint32_t)0xFF007FE3
#define MOTOR_CONFIG_ADC_I1_SCALE_OFFSET (uint32_t)0xFF0080E9
#elif MOTOR == 2
#define MOTOR_CONFIG_ADC_I_SELECT (uint32_t)0x24000100
#define MOTOR_CONFIG_dsADC_MCFG_B_MCFG_A (uint32_t)0x00100010
#define MOTOR_CONFIG_dsADC_MCLK_A (uint32_t)0x20000000
#define MOTOR_CONFIG_dsADC_MCLK_B (uint32_t)0x00000000
#define MOTOR_CONFIG_dsADC_MDEC_B_MDEC_A (uint32_t)0x014E014E
#define MOTOR_CONFIG_ADC_I0_SCALE_OFFSET (uint32_t)0x010081C9
#define MOTOR_CONFIG_ADC_I1_SCALE_OFFSET (uint32_t)0x010081AE
#endif

/*********************************************************************************
 *
 * 		Digital hall Parameters
 *
 **********************************************************************************/

#if MOTOR == 0
#define MOTOR_CONFIG_HALL_MODE (uint32_t)0x00000001
#define MOTOR_CONFIG_HALL_PHI_E_PHI_M_OFFSET (uint32_t)0x00000000
#elif MOTOR == 1
#define MOTOR_CONFIG_HALL_MODE (uint32_t)0x00000001
#define MOTOR_CONFIG_HALL_PHI_E_PHI_M_OFFSET (uint32_t)0x00000000
#elif MOTOR == 2
#define MOTOR_CONFIG_HALL_MODE (uint32_t)0x00000001
#define MOTOR_CONFIG_HALL_PHI_E_PHI_M_OFFSET (uint32_t)0xEC780000
#endif
/*********************************************************************************
 *
 * 		Selectors
 *
 *		MOTOR_INIT_MODE 0 -> Use open loop to initialize the encoder effset.
 *		MOTOR_INIT_MODE 2 -> Use hall sensors to initialize the encoder offset.
 *
 **********************************************************************************/

#if MOTOR == 0
#define MOTOR_CONFIG_PHI_E_SELECTION (uint32_t)0x00000005
#define MOTOR_CONFIG_VELOCITY_SELECTION (uint32_t)0x0000000C
#define MOTOR_INIT_MODE 0
#elif MOTOR == 1
#define MOTOR_CONFIG_PHI_E_SELECTION (uint32_t)0x00000003
#define MOTOR_CONFIG_VELOCITY_SELECTION (uint32_t)0x00000003
#define MOTOR_INIT_MODE 2
#elif MOTOR == 2

#define MOTOR_CONFIG_STARTING_PHI_E_SELECTION (uint32_t)0x00000005
#define MOTOR_CONFIG_TARGET_PHI_E_SELECTION (uint32_t)0x00000003

#define MOTOR_CONFIG_PHI_E_SELECTION (uint32_t)0x00000003
#define MOTOR_CONFIG_VELOCITY_SELECTION (uint32_t)0x00000009
#define MOTOR_INIT_MODE 2
#endif

/*********************************************************************************
 *
 * 		Limit Parameters
 *
 *		MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS is the maximum mA that the motor can draw
 *		MIN_VELOCITY and MAX_VELOCITY are absolute values.
 *
 **********************************************************************************/

#define MIN_VELOCITY 0

#if MOTOR == 0
#define MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS (uint32_t)2000
#define MAX_VELOCITY 4000
#elif MOTOR == 1
#define MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS (uint32_t)1000
#define MAX_VELOCITY 3000
#elif MOTOR == 2

#if MOTOR_CONFIG_MODE_RAMP_MODE_MOTION == 2
	#define MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS (uint32_t)25000
#else
	#define MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS (uint32_t)32000
#endif

#define MOTOR_CONFIG_PID_TORQUE_FLUX_THROTTLE_LIMITS (uint32_t)28000
#define MOTOR_CONFIG_PID_UQ_UD_LIMITS 28000
#define MAX_VELOCITY 3000
#define MOTOR_CONFIG_ACCELERATION_LIMIT 300
#endif
/*********************************************************************************
 *
 * 		PI Parameters
 *
 * 		Set PI values. Very important for proper behaviour.
 *
 **********************************************************************************/

#if MOTOR == 0
#define MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I (uint32_t)0x01000100
#define MOTOR_CONFIG_PID_FLUX_P_FLUX_I (uint32_t)0x01000100
#define MOTOR_CONFIG_PID_VELOCITY_P_VELOCITY_I (uint32_t)0x000F0000
#elif MOTOR == 1
#define MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I (uint32_t)0x01000100
#define MOTOR_CONFIG_PID_FLUX_P_FLUX_I (uint32_t)0x01000100
#define MOTOR_CONFIG_PID_VELOCITY_P_VELOCITY_I (uint32_t)0x00000000
#elif MOTOR == 2
#define MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I (uint32_t)0x0041001E
#define MOTOR_CONFIG_PID_FLUX_P_FLUX_I (uint32_t)0x0041001E
#define MOTOR_CONFIG_PID_VELOCITY_P_VELOCITY_I (uint32_t)0x6D600100
#endif
/*********************************************************************************
 *
 * 		ABN Decoder Parameters
 *
 * 		#define ABN -> Enable ABN in code. Preserve parameters and disable ABN by removing this define.
 *
 **********************************************************************************/

#if MOTOR == 0

#elif MOTOR == 1

/**
 * Defining ABN will add extra lines of code to init to enable ABN encoder
 */
#define ABN

#define MOTOR_CONFIG_ABN_DECODER_MODE (uint32_t)0x00000000     // 0x25: Polarity of A pulse.
#define MOTOR_CONFIG_ABN_DECODER_PPR (uint32_t)0x00000B40      // 0x26: Decoder pulses per mechanical revolution.
#define MOTOR_CONFIG_ABN_DECODER_COUNT (uint32_t)0x00000A26    // 0x27: Raw decoder count; the digital decoder engine counts modulo (decoder_ppr).
#define MOTOR_CONFIG_ABN_DECODER_COUNT_N (uint32_t)0x0000096C  // 0x28: Decoder count latched on N pulse, when N pulse clears decoder_count also decoder_count_n is 0.
#define MOTOR_CONFIG_ABN_DECODER_PHI_E_PHI_M_OFFSET 0x00000000 // 0x29: ABN_DECODER_PHI_M_OFFSET to shift (rotate) angle DECODER_PHI_M.
#elif MOTOR == 2

#define ABN

#define MOTOR_CONFIG_ABN_DECODER_MODE (uint32_t)0x00000000     // 0x25: Polarity of A pulse.
#define MOTOR_CONFIG_ABN_DECODER_PPR (uint32_t)0x00000B40      // 0x26: Decoder pulses per mechanical revolution.
#define MOTOR_CONFIG_ABN_DECODER_COUNT (uint32_t)0x00000A26    // 0x27: Raw decoder count; the digital decoder engine counts modulo (decoder_ppr).
#define MOTOR_CONFIG_ABN_DECODER_COUNT_N (uint32_t)0x00000494  // 0x28: Decoder count latched on N pulse, when N pulse clears decoder_count also decoder_count_n is 0.
#define MOTOR_CONFIG_ABN_DECODER_PHI_E_PHI_M_OFFSET 0xC1800000 // 0x29: ABN_DECODER_PHI_M_OFFSET to shift (rotate) angle DECODER_PHI_M.
#endif
/*********************************************************************************
 *
 * 		Miscelanious Parameters.
 *
 **********************************************************************************/

#define POSITION_SCALE_MAX (int32_t)65536
#define MOTOR_CONFIG_TORQUE_MESUREMENT_FACTOR (uint32_t)256

/*********************************************************************************
 *
 * 		Driver Parameters (TMC 6200)
 *
 **********************************************************************************/

#if MOTOR == 0
/**
 * General Configuration Flags for the TMC6200
 * Bit 0: disable (Driver Disable). Cycle low to clear short conditions.
 */
#define MOTOR_CONFIG_DRIVER_GENERAL_CONFIG (uint32_t)0x00000000

/**
 * Short detection / Safety parameters
 * 0x13010606 is the factory default value
 */
#define MOTOR_CONFIG_DRIVER_SHORT_CONFIG (uint32_t)0x13010606

/**
 * Drive parameters
 * 0x00080004 is the factory default value
 */
#define MOTOR_CONFIG_DRIVER_DRIVE_CONFIG (uint32_t)0x00080004
#elif MOTOR == 1
/**
 * General Configuration Flags for the TMC6200
 * Bit 0: disable (Driver Disable). Cycle low to clear short conditions.
 */
#define MOTOR_CONFIG_DRIVER_GENERAL_CONFIG (uint32_t)0x00000000

/**
 * Short detection / Safety parameters
 * 0x13010606 is the factory default value
 */
#define MOTOR_CONFIG_DRIVER_SHORT_CONFIG (uint32_t)0x13010606

/**
 * Drive parameters
 * 0x00080004 is the factory default value
 */
#define MOTOR_CONFIG_DRIVER_DRIVE_CONFIG (uint32_t)0x00080004
#elif MOTOR == 2
/**
 * General Configuration Flags for the TMC6200
 * Bit 0: disable (Driver Disable). Cycle low to clear short conditions.
 */
#define MOTOR_CONFIG_DRIVER_GENERAL_CONFIG (uint32_t)0x00000000

/**
 * Short detection / Safety parameters
 * 0x13010606 is the factory default value
 */
#define MOTOR_CONFIG_DRIVER_SHORT_CONFIG (uint32_t)0x13010606

/**
 * Drive parameters
 * 0x00080004 is the factory default value
 */
#define MOTOR_CONFIG_DRIVER_DRIVE_CONFIG (uint32_t)0x00040004

#define MOTOR_CLEAR_CHARGE_PUMP_FAULT
#endif
#endif

/* INC_MOTORPARAMETERS_H_ */
