/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */


#include "DataAggregationModule.h"
#include "MotorTask.h"

#include "SerialDebugDriver.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityRealtime1
#define TIMER_MOTOR_TASK 500UL
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

PUBLIC void InitMotorTask(void)
{

	MotorTaskHandle = osThreadNew(MotorTask, NULL, &MotorTask_attributes);

}
PRIVATE void MotorTask(void *argument)
{
	uint32_t cycleTick = osKernelGetTickCount();
	DebugPrint("%s Initializing MotorTask", MOT_TAG);

	uint32_t motorInitialized = initMotor();

	if (!motorInitialized) {
		SystemSetSPIError(Set);
		DebugPrint("%s Failed to initialize motor!", MOT_TAG);
	}

	for(;;)
	{
		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

		if (motorInitialized) {
			DebugPrint("%s Target Velocity [%x]", SystemGetTargetVelocity(), MOT_TAG);
			rotate(SystemGetTargetVelocity());
			periodicJob(cycleTick);
		} else {
			SystemSetSPIError(Set);
			DebugPrint("%s Failed to initialize motor!", MOT_TAG);

			// If motor failed to initialize, wait and then reinit
			osDelay(TIMER_MOTOR_REINIT_DELAY);
			motorInitialized = initMotor();
		}

	}
}
