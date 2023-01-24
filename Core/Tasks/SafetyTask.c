/*
 * SafetyTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 *
 * 	This is the module for managing safety devices such as DM switch
 *  Can only import interface files
 */

#include "DataAggregationModule.h"
#include "SafetyTask.h"

#include "stdint.h"

#include "SerialDebugDriver.h"

#include "SPIMotorDriver.h"

#include "LEDStatusDriver.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define SAFETY_TASK_PRIORITY (osPriority_t) osPriorityHigh
#define TIMER_SAFETY_TASK 5000UL

const char SFT_TAG[] = "#SFT:";

PUBLIC void InitSafetyTask(void);
PRIVATE void SafetyTask(void *argument);

osThreadId_t SafetyTaskHandle;
const osThreadAttr_t SafetyTask_attributes = {
	.name = "SafetyTask",
	.stack_size = STACK_SIZE,
	.priority = SAFETY_TASK_PRIORITY,
};

PUBLIC void InitSafetyTask(void)
{

	SafetyTaskHandle = osThreadNew(SafetyTask, NULL, &SafetyTask_attributes);

}
PRIVATE void SafetyTask(void *argument)
{
	uint32_t cycleTick = osKernelGetTickCount();
	DebugPrint("%s safety", SFT_TAG);

	for(;;)
	{
		cycleTick += TIMER_SAFETY_TASK;
		osDelayUntil(cycleTick);
		DebugPrint("%s Safety Error [%d],  SPI Error [%d], iComms Error [%d]", SFT_TAG, SystemGetSafetyError(), SystemGetSPIError(), SystemGetiCommsError());

		if (MotorValidateSPI()) {
			SystemSetSPIError(Clear);
		} else {
			SystemSetSPIError(Set);
		}

		STATUS_PeriodicDisplayStatusCode(SystemGetStatusCode());
	}
}
