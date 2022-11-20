/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */


#include "MotorTask.h"

#include <stdint.h>

#include "SerialDebugDriver.h"

#include "tmc/ic/TMC4671/TMC4671.h"

extern MotorConfigTypeDef motorConfig;

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityHigh1
#define TIMER_MOTOR_TASK 2000UL

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


	initMotor();

	uint8_t c = 0;
	for(;;)
	{
		DebugPrint("Motor loop");
		DebugPrint("Torque: %d", motorConfig.targetTorque);



		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

		switch(c) {
		case 0:
			DebugPrint("Rotate Right");
			setTargetTorque(0x03E80000);
			c = 1;
			break;
		case 1:
			DebugPrint("Rotate Left");
			setTargetTorque(0xFC180000);
			c = 2;
			break;
		default:
			c = 0;
			DebugPrint("Stop");
			setTargetTorque(0x00000000);
			break;
		}
	}
}
