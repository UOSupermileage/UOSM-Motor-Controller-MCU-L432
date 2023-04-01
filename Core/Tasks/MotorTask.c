/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */


#include "DataAggregationModule.h"
#include "MotorTask.h"
#include "MotorParameters.h"
#include "SerialDebugDriver.h"
#include "Profiles.h"
#include "tmc/ic/TMC4671/TMC4671.h"
#include "tmc/ic/TMC6200/TMC6200.h"
#include "tmc/ic/TMC6200/TMC6200_Register.h"

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
	uint32_t motorInitialized = 0;

	uint32_t cycleTick = osKernelGetTickCount();
	DebugPrint("%s Initializing MotorTask", MOT_TAG);

	motorInitialized = MotorInit();


#ifdef MOTOR_PROFILE
	motorInitialized = MotorInit();

	if (!motorInitialized) {
		SystemSetSPIError(Set);
		DebugPrint("%s Failed to initialize motor!", MOT_TAG);
	}
#else
	// If motor is set to idle, just enable and do nothing else.
//	MotorSetCS(TMC4671_CS, GPIO_PIN_SET);
//	MotorSetCS(TMC6200_CS, GPIO_PIN_SET);
//	MotorEnableDriver(ENABLED);
//	HAL_Delay(500);
//	MotorClearChargePump();
#endif

	for(;;)
	{
		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

#ifdef MOTOR_FIXED_THROTTLE
//		SystemSetThrottlePercentage(MOTOR_FIXED_THROTTLE);
		SystemSetTargetVelocity(1000);
#endif

#ifdef MOTOR_PROFILE
		if (motorInitialized) {

			DebugPrint("%s Target Velocity [%x]", MOT_TAG,  MOTOR_FIXED_THROTTLE);
			MotorRotate(SystemGetTargetVelocity());
			MotorPeriodicJob(cycleTick);
		} else {
			SystemSetSPIError(Set);
			DebugPrint("%s Failed to initialize motor!", MOT_TAG);

			// If motor failed to initialize, wait and then reinit
			osDelay(TIMER_MOTOR_REINIT_DELAY);
			motorInitialized = MotorInit();
		}
#else
		MotorPrintFaults();
		// If motor is set to idle, remind user.
//		DebugPrint("Motor Parameters Set To Idle... Change in MotorParamters.h");

#endif
	}
}
