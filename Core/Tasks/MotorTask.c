/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#include "MotorTask.h"
#include "DataAggregationModule.h"
#include "MotorParameters.h"
#include "Profiles.h"
#include "SerialDebugDriver.h"
#include "tmc/ic/TMC4671/TMC4671.h"
#include "tmc/ic/TMC6200/TMC6200.h"
#include "tmc/ic/TMC6200/TMC6200_Register.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128 * 4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityHigh3
#define TIMER_MOTOR_TASK 150UL
#define TIMER_MOTOR_REINIT_DELAY 5000UL

const char MOT_TAG[] = "#MOT:";

PUBLIC void InitMotorTask(void);
PRIVATE void MotorTask(void *argument);

osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
    .name = "MotorTask",
    .stack_size = STACK_SIZE,
    .priority = MOTOR_TASK_PRIORITY,
};

PUBLIC void InitMotorTask(void) {
#ifdef MOTOR_PROFILE
    MotorTaskHandle = osThreadNew(MotorTask, NULL, &MotorTask_attributes);
#endif
}
/// [task]
/**
 * Execution Loop for Motor Task.
 */
PRIVATE void MotorTask(void *argument) {
    // Keep track of current tick count
	uint32_t cycleTick = osKernelGetTickCount();

	// Initialize the system as configured in MotorParameters.h
    DebugPrint("%s Initializing MotorTask", MOT_TAG);
    uint32_t motorInitialized = MotorInit();

    for (;;) {
    	// Increment cycleTick and wait until the delay has passed
        cycleTick += TIMER_MOTOR_TASK;
        osDelayUntil(cycleTick);

        DebugPrint("Motor Task");

		#if MOTOR_MODE == 0 || MOTOR_MODE == 1

			#ifdef MOTOR_FIXED_THROTTLE
        			// Set throttle on each execution of loop to ensure throttle is not changed
					SystemSetThrottlePercentage(MOTOR_FIXED_THROTTLE);
			#endif

			if (motorInitialized) {
				#if MOTOR_MODE == 0
					#if MOTOR_CONFIG_MODE_RAMP_MODE_MOTION == 1
						DebugPrint("%s Target Torque [%d mA]", MOT_TAG, SystemGetThrottlePercentage() * MOTOR_CONFIG_PID_TORQUE_FLUX_THROTTLE_LIMITS / 1000);
						MotorRotateTorque(SystemGetThrottlePercentage() * MOTOR_CONFIG_PID_TORQUE_FLUX_THROTTLE_LIMITS / 1000);
					#elif MOTOR_CONFIG_MODE_RAMP_MODE_MOTION == 2
						DebugPrint("%s Target Velocity [%d RPM]", MOT_TAG, SystemGetTargetVelocity());
						MotorRotate(SystemGetTargetVelocity());
					#endif

					MotorPeriodicJob(cycleTick);
				#endif
			} else {
				// Motor was not initialized. This indicates that communication with the TMC4671 or TMC6200 failed.
				SystemSetSPIError(Set);
				DebugPrint("%s Failed to initialize motor!", MOT_TAG);

				// Motor failed to initialize, wait and then reinit
				osDelay(TIMER_MOTOR_REINIT_DELAY);
				motorInitialized = MotorInit();
//				DebugPrint("%s Read DRV_CONF: %x", MOT_TAG, tmc6200_readInt(TMC6200_CS, TMC6200_DRV_CONF));
//				DebugPrint("%s Read HALL_MODE: %x", MOT_TAG, tmc4671_readInt(TMC4671_CS, TMC4671_HALL_MODE));
			}
		#endif

	#ifdef VERBOSE
			// Read registers in TMC6200 and check for faults
			MotorPrintFaults();
	#endif
    }
/// [task]
}
