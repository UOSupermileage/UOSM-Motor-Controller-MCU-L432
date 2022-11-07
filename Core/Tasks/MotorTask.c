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
		DebugPrint("Loop");

		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

		DebugPrint("Motor loop");
//		tmc4671_readInt(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS);
//		tmc4671_writeInt(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0x03E80000);
//		HAL_Delay(50);
//		tmc4671_writeInt(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, 0xFFFFFFFF);
//		tmc4671_readwriteByte(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS |0x80, 0x00);
//		tmc4671_writeInt(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0xFFFFFFFF);

//		tmc4671_readInt(0, TMC4671_PID_TORQUE_FLUX_TARGET);

		if (c == 0) {

			DebugPrint("Rotate Left");
			rotateMotorLeft();
			c = 1;
		} else {
			DebugPrint("Rotate Right");
			rotateMotorRight();
			c = 0;
		}
	}
}
