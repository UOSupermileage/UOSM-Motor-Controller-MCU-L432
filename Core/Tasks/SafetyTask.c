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

uint8_t LED_Number_Per_Ring = 60;
uint32_t Ring_0_Display_memory[60];

void InitSafetyTask(void)
{
    SafetyTaskHandle = osThreadNew(SafetyTask, NULL, &SafetyTask_attributes);
}

void Embeded_One(void)
{
    HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, GPIO_PIN_SET);
    // Adjust the delay to meet the timing requirements of the LED
    for (volatile int i = 0; i < 10; i++);
    HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, GPIO_PIN_RESET);
    for (volatile int i = 0; i < 5; i++);
}

void Embeded_Zero(void)
{
    HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, GPIO_PIN_SET);
    // Adjust the delay to meet the timing requirements of the LED
    for (volatile int i = 0; i < 5; i++);
    HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, GPIO_PIN_RESET);
    for (volatile int i = 0; i < 10; i++);
}

void Send_Whole_Ring_from_Ring_Memory(void){
    uint8_t j=0;
    uint32_t x,y;
    for (uint8_t i=0; i<LED_Number_Per_Ring; i++)
    {
        y = Ring_0_Display_memory[i];
        for (j=0; j<24; j++)
        {
            x = (y & 0x800000);
            if (x>0)
                Embeded_One();
            else
                Embeded_Zero();
            y = y << 1;
        }
    }
    HAL_Delay(80);
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
        uint32_t SafetyErrors = SystemGetSafetyError();
        uint32_t SPIErrors = SystemGetSPIError();
        uint32_t iCommsErrors = SystemGetiCommsError();
        uint32_t throttleErrors = SystemGetThrottleError();
        DebugPrint("%s Safety Error [%d],  SPI Error [%d], iComms Error [%d], Throttle Error [%d]", SFT_TAG, SafetyErrors, SPIErrors, iCommsErrors, throttleErrors);

        static uint8_t s = 0;

        HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, s);

        // Green means All Good
        // Red means Safety Error
        // Teal means SPI Error
        // Yellow means iCommsError
        // Purple means Throttle Error
            if (SafetyErrors == 0 &&  SPIErrors == 0 &&  iCommsErrors == 0 && throttleErrors == 0) {
                HAL_Delay(1000);

                // Ring_0 takes a hex value for colour
                Ring_0_Display_memory[0] = 0x00FF00;

                Send_Whole_Ring_from_Ring_Memory();
            } else {
                if (SafetyErrors >= 1) {
                    Ring_0_Display_memory[0] = 0xFF0000;
                    Send_Whole_Ring_from_Ring_Memory();
                    HAL_Delay(1000);
                }
                if (SPIErrors >= 1) {
                    Ring_0_Display_memory[1] = 0x008080;
                    Send_Whole_Ring_from_Ring_Memory();
                    HAL_Delay(1000);
                }
                if (iCommsErrors >= 1) {
                    Ring_0_Display_memory[2] = 0xFFFF00;
                    Send_Whole_Ring_from_Ring_Memory();
                    HAL_Delay(1000);
                }
                if (throttleErrors >= 1) {
                    Ring_0_Display_memory[3] = 0xA020F0;
                    Send_Whole_Ring_from_Ring_Memory();
                    HAL_Delay(1000);
                }
            }

        s = !s;
#endif
    }
}