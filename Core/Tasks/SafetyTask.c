/*
 * SafetyTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 *
 * 	This is the module for managing safety devices such as DM switch
 *  Can only import interface files
 */

#include "SafetyTask.h"

#include <stdint.h>

#include "SerialDebugDriver.h"

#include "SPIMotorDriver.h"
#include "DatastoreModule.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define SAFETY_TASK_PRIORITY (osPriority_t) osPriorityHigh
#define TIMER_SAFETY_TASK 10000UL

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
	DebugPrint("safety");

	for(;;)
	{
		cycleTick += TIMER_SAFETY_TASK;
		osDelayUntil(cycleTick);
		DebugPrint("Safety Loop. Safety Error [%d],  SPI Error [%d], iComms Error [%d]", datastoreGetSafetyError(), datastoreGetSPIError(), datastoreGetiCommsError());

		if (validateSPI()) {
			datastoreSetSPIError(Clear);
		} else {
			datastoreSetSPIError(Set);
		}

	}
}
