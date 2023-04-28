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
#include "tmc/ic/TMC4671/TMC4671.h"
#include "tmc/ic/TMC6200/TMC6200.h"
#include "tmc/ic/TMC6200/TMC6200_Register.h"

#define STACK_SIZE 128 * 4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityRealtime
#define TIMER_MOTOR_TASK 150UL
#define TIMER_MOTOR_REINIT_DELAY 100UL

const char MOT_TAG[] = "#MOT:";

PUBLIC void InitMotorTask(void);
PRIVATE void MotorTask(void *argument);

osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
	.name = "MotorTask",
	.stack_size = STACK_SIZE,
	.priority = MOTOR_TASK_PRIORITY,
};

PUBLIC void InitMotorTask(void)
{
#ifdef PROFILE_MOTOR
	MotorTaskHandle = osThreadNew(MotorTask, NULL, &MotorTask_attributes);
#endif
}
/// [task]
/**
 * Execution Loop for Motor Task.
 */
PRIVATE void MotorTask(void *argument)
{
	// Keep track of current tick count
	uint32_t cycleTick = osKernelGetTickCount();

	// Initialize the system as configured in MotorParameters.h
	DebugPrint("%s Initializing MotorTask", MOT_TAG);
	uint32_t motorInitialized = MotorInit();

	for (;;)
	{
		// Increment cycleTick and wait until the delay has passed
		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

		DebugPrint("Motor Task: Motor mode [%d]", SystemGetMotorMode());

		if (SystemGetMotorMode() == MOTOR_MODE_NORMAL || SystemGetMotorMode() == MOTOR_MODE_RTMI) {

			if (motorInitialized) {

				switch (SystemGetMotionMode()) {
					case TMC4671_MOTION_MODE_TORQUE:
						;
						torque_t torque = SystemGetThrottlePercentage() * MOTOR_CONFIG_PID_TORQUE_FLUX_THROTTLE_LIMITS / 1000;
						
						if (SystemGetReverseVelocity() == Set) {
							torque *= -1;
						}
						
						DebugPrint("%s Target Torque [%d mA]", MOT_TAG, torque);
						MotorRotateTorque(torque);
						break;
					case TMC4671_MOTION_MODE_VELOCITY:
						;
						velocity_t v = (MAX_VELOCITY) / MAX_PERCENTAGE * SystemGetThrottlePercentage();

						if (SystemGetReverseVelocity() == Set) {
							v *= -1;
						}

						DebugPrint("%s Target Velocity [%d RPM]", MOT_TAG, v);
						MotorRotateVelocity(v);
						break;
					case TMC4671_MOTION_MODE_POSITION:
						;
						torque_t positionTorque = SystemGetThrottlePercentage() * MOTOR_CONFIG_PID_TORQUE_FLUX_THROTTLE_LIMITS / 1000;
						DebugPrint("%s Dynamo Torque [%d mA]", MOT_TAG, positionTorque);
						MotorRotatePosition(positionTorque);
						break;
				}

				MotorPeriodicJob(cycleTick);
			} else {
				// Motor was not initialized. This indicates that communication with the TMC4671 or TMC6200 failed.
				SystemSetSPIError(Set);
				DebugPrint("%s Failed to initialize motor!", MOT_TAG);

				// Motor failed to initialize, wait and then reinit
				osDelay(TIMER_MOTOR_REINIT_DELAY);
				motorInitialized = MotorInit();
			}

		} else {
			DebugPrint("Motor Idle...");
		}

		// Read registers in TMC6200 and check for faults
		MotorPrintFaults();
	}
	/// [task]
}
