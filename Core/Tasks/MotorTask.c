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
#include "tmc/ic/TMC6200/TMC6200.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityHigh1
#define TIMER_MOTOR_TASK 1000UL
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

	uint32_t motorInitialized = initMotor();

	if (!motorInitialized) {
		datastoreSetSPIError(Set);
		DebugPrint("Failed to initialize motor!");
	}

//	uint32_t counter = 0;

	for(;;)
	{

//		counter++;
//
//		if (counter <= 15) {
////			datastoreSetTargetTorquePercentage(5);
//			datastoreSetTargetVelocity(50000000);
//		}else if (counter <= 30) {
////			datastoreSetTargetTorquePercentage(80);
//			datastoreSetTargetVelocity(0);
//			counter = 0;
//		} else if (counter <= 45) {
////			datastoreSetTargetTorquePercentage(50);
//		} else if (counter <= 60) {
////			datastoreSetTargetTorquePercentage(0);
//			counter = 0;
//		}

//		 datastoreSetThrottlePercentage(500);

		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

		if (motorInitialized) {
			//		writeTargetTorque(datastoreGetTargetTorque());
			DebugPrint("Target Velocity [%x]", datastoreGetTargetVelocity());
			 rotate(datastoreGetTargetVelocity());
			periodicJob(cycleTick);
		} else {
			// If motor failed to initialize, wait and then reinit
			osDelay(TIMER_MOTOR_REINIT_DELAY);
			motorInitialized = initMotor();
		}

	}
}
