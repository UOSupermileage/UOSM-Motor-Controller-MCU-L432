/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */


#include "MotorTask.h"

#include <stdint.h>

#include "SerialDebugDriver.h"

extern SPI_HandleTypeDef hspi1;

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityHigh1
#define TIMER_MOTOR_TASK 1000UL

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
	DebugPrint("motor");

	uint16_t size = 10;
	uint8_t message[10] = "motor\n\r"; // unsigned 8 bit integer array (utf8 = 8 bit int per char)

	for(;;)
	{
		cycleTick += TIMER_MOTOR_TASK;
		osDelayUntil(cycleTick);

		DebugPrint("Sending message to motor controller:");
		DebugPrint(message);
		HAL_SPI_Transmit(&hspi1, message, size, 100);
		DebugPrint("Message sent");
	}
}
