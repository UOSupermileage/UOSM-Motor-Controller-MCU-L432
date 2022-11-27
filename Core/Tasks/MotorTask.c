/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */


#include "MotorTask.h"

#include "SerialDebugDriver.h"
#include "DatastoreModule.h"

#include "tmc/ic/TMC4671/TMC4671.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityHigh1
#define TIMER_MOTOR_TASK 2000UL
#define TIMER_MOTOR_REINIT_DELAY 5000UL

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
	DebugPrint("Initializing MotorTask");


//	datastoreSetTargetTorque(0x03E80000);

	bool motorInitialized = initMotor();

	if (!motorInitialized) {
		datastoreSetSPIError(Set);
	}

	for(;;)
	{
		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

		writeTargetTorque(datastoreGetTargetTorque());

//		if (motorInitialized) {
//			writeTargetTorque(datastoreGetTargetTorque());
//		}else {
//			// If motor failed to initialize, wait and then reinit
//			osDelay(TIMER_MOTOR_REINIT_DELAY);
//			motorInitialized = initMotor();
//		}

	}
}
