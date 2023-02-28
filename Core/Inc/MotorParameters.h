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

/**
 * Select Target Motor
 * 0 == QBL4208-81-04-019 (Tiny Motor)
 * 1 == BLK322D-48V-3000 (48V Motor that runs at 3000 RPM)
 */
#define MOTOR 1

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
 **********************************************************************************/

#if MOTOR == 0
	#define MOTOR_CONFIG_N_POLE_PAIRS (uint32_t)0x00030004
#elif MOTOR == 1
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
#endif

/*********************************************************************************
 *
 * 		Digital hall Parameters
 *
 * 		HALL_MODE = Mode for digital hall sensors uses as position decoder.
 *
 **********************************************************************************/

#if MOTOR == 0
	#define MOTOR_CONFIG_HALL_MODE (uint32_t)0x00000001
	#define MOTOR_CONFIG_HALL_PHI_E_PHI_M_OFFSET (uint32_t)0x00000000
#elif MOTOR == 1
	#define MOTOR_CONFIG_HALL_MODE (uint32_t)0x00000001
	#define MOTOR_CONFIG_HALL_PHI_E_PHI_M_OFFSET (uint32_t)0x00000000
#endif

/*********************************************************************************
 *
 * 		Selectors
 *
 **********************************************************************************/

#if MOTOR == 0
	#define MOTOR_CONFIG_PHI_E_SELECTION (uint32_t)0x00000005
	#define MOTOR_CONFIG_VELOCITY_SELECTION (uint32_t)0x0000000C
	#define MOTOR_INIT_MODE 0
#elif MOTOR == 1
	#define MOTOR_CONFIG_PHI_E_SELECTION (uint32_t)0x00000003
	#define MOTOR_CONFIG_VELOCITY_SELECTION (uint32_t)0x00000003
	#define MOTOR_INIT_MODE 2 // 2 = use hall sensor signals to init
#endif

/*********************************************************************************
 *
 * 		Limit Parameters
 *
 **********************************************************************************/

#define MIN_VELOCITY 0

#if MOTOR == 0
	#define MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS (uint32_t) 1000 // in mA
	#define MAX_VELOCITY 4000
#elif MOTOR == 1
	#define MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS (uint32_t) 1000 // in mA
	#define MAX_VELOCITY 3000
#endif

/*********************************************************************************
 *
 * 		PI Parameters
 *
 **********************************************************************************/

#if MOTOR == 0
	#define MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I (uint32_t)0x01000100
	#define MOTOR_CONFIG_PID_FLUX_P_FLUX_I (uint32_t)0x01000100
#elif MOTOR == 1
	#define MOTOR_CONFIG_PID_TORQUE_P_TORQUE_I (uint32_t)0x01000100
	#define MOTOR_CONFIG_PID_FLUX_P_FLUX_I (uint32_t)0x01000100
#endif

/*********************************************************************************
 *
 * 		ABN Decoder Parameters
 *
 **********************************************************************************/

#if MOTOR == 0

#elif MOTOR == 1

	/**
	 * Defining ABN will add extra lines of code to init to enable ABN encoder
	 */
	#define ABN

	#define MOTOR_CONFIG_ABN_DECODER_MODE (uint32_t)0x00000000 //0x25: Polarity of A pulse.
	#define MOTOR_CONFIG_ABN_DECODER_PPR (uint32_t) 0x00000B40 //0x26: Decoder pulses per mechanical revolution.
	#define MOTOR_CONFIG_ABN_DECODER_COUNT (uint32_t) 0x00000A26 //0x27: Raw decoder count; the digital decoder engine counts modulo (decoder_ppr).
	#define MOTOR_CONFIG_ABN_DECODER_COUNT_N (uint32_t) 0x0000096C //0x28: Decoder count latched on N pulse, when N pulse clears decoder_count also decoder_count_n is 0.
	#define MOTOR_CONFIG_ABN_DECODER_PHI_E_PHI_M_OFFSET 0x00000000 //0x29: ABN_DECODER_PHI_M_OFFSET to shift (rotate) angle DECODER_PHI_M.
	#define MOTOR_CONFIG_ABN_DECODER_PHI_E_PHI_M 0x5FA417E9 //0x2A: ABN_DECODER_PHI_M = ABN_DECODER_COUNT * 2^16 / ABN_DECODER_PPR + ABN_DECODER_PHI_M_OFFSET;

#endif


/*********************************************************************************
 *
 * 		Motion Mode Parameters
 *
 **********************************************************************************/

#define MOTOR_CONFIG_MODE_RAMP_MODE_MOTION (uint32_t)0x00000002 // Velocity

/*********************************************************************************
 *
 * 		Miscelanious Parameters. Unused, but may be useful later
 *
 **********************************************************************************/

#define POSITION_SCALE_MAX  (int32_t)65536
#define MOTOR_CONFIG_TORQUE_MESUREMENT_FACTOR (uint32_t) 256

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
#endif


#endif /* INC_MOTORPARAMETERS_H_ */
