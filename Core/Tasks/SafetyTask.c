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

#include "SPIMotorDriver.h"

//#include "LEDStatusDriver.h"
#include "Profiles.h"
#include "main.h"

#define STACK_SIZE 128*4
#define SAFETY_TASK_PRIORITY (osPriority_t) osPriorityHigh
#define TIMER_SAFETY_TASK 1000UL

const char SFT_TAG[] = "#SFT:";

void InitSafetyTask(void);
void SafetyTask(void *argument);

osThreadId_t SafetyTaskHandle;
const osThreadAttr_t SafetyTask_attributes = {
	.name = "SafetyTask",
	.stack_size = STACK_SIZE,
	.priority = SAFETY_TASK_PRIORITY,
};

void InitSafetyTask(void)
{
	SafetyTaskHandle = osThreadNew(SafetyTask, NULL, &SafetyTask_attributes);
}

void SafetyTask(void *argument)
{
	uint32_t cycleTick = osKernelGetTickCount();
	DebugPrint("%s safety", SFT_TAG);

	for(;;)
	{
		cycleTick += TIMER_SAFETY_TASK;
		osDelayUntil(cycleTick);

#ifdef PROFILE_SAFETY
		DebugPrint("%s Safety Error [%d],  SPI Error [%d], iComms Error [%d]", SFT_TAG, SystemGetSafetyError(), SystemGetSPIError(), SystemGetiCommsError());

		static uint8_t s = 0;

		HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, s);
	    // If Safety Error, flash red, GetSPIE, flash orange, and Comms Flash blue
	    // If multiple errors, flash the applicable ones
	    // If no errors, flash green
	        if (SystemGetSafetyError() >= 1) {
			HAL_GPIO_WritePin(Safety_LED_GPIO_Port, Safety_LED_Pin, 1);
		} else {
			HAL_GPIO_WritePin(Safety_LED_GPIO_Port, Safety_LED_Pin, 0);
		}

	        if (SystemGetSPIError() >= 1) {
			    HAL_GPIO_WritePin(SPI_LED_GPIO_Port, SPI_LED_Pin, 1);
		    } else {
			    HAL_GPIO_WritePin(SPI_LED_GPIO_Port, SPI_LED_Pin, 0);
		    }

	        if (SystemGetiCommsError() >= 1) {
			    HAL_GPIO_WritePin(iComms_LED_GPIO_Port, iComms_LED_Pin, 1);
		    } else {
			    HAL_GPIO_WritePin(iComms_LED_GPIO_Port, iComms_LED_Pin, 0);
		    }

		s = !s;
#endif
	}
}
