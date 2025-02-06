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

void Init_GPIOs(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Embeded_One(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

void Embeded_Zero(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

void Send_Whole_Ring_from_Ring_Memory(void){
    uint8_t j=0;
    uint32_t x,y;
    for (i=0;i<LED_Number_Per_Ring+10;i++)
    {
        y = Ring_0_Display_memory[i];
        for (j=0;j<8;j++)
        {
            x = (y & 0x800000);
            if (x>0)
                Embeded_One();
            else
                Embeded_Zero();
            y = y << 1;
        }
        y = Ring_0_Display_memory[i];
        for (j=0;j<8;j++)
        {
            x = (y & 0x008000);
            if (x>0)
                Embeded_One();
            else
                Embeded_Zero();
            y = y << 1;
        }
        y = Ring_0_Display_memory[i];
        for (j=0;j<8;j++)
        {
            x = (y & 0x000080);
            if (x>0)
                Embeded_One();
            else
                Embeded_Zero();
            y = y << 1;
        }
    }
    delay_us(80);



void SafetyTask(void *argument)
{
	uint32_t cycleTick = osKernelGetTickCount();
        uint8_t LED_Number_Per_Ring = 60;
        uint32_t Ring_0_Display_memory[60];

        void Display_One_Dot(uint32_t color);
        void Embeded_One(void);
        void Embeded_Zero(void);
        void Init_GPIOs(void);
	DebugPrint("%s safety", SFT_TAG);

	for(;;)
	{
		cycleTick += TIMER_SAFETY_TASK;
		osDelayUntil(cycleTick);

#ifdef PROFILE_SAFETY
	    uint32_t SafetyErrors = SystemGetSafetyError();
	    uint32_t SPIErrors = SystemGetSPIError();
	    uint32_t iCommsErrors = SystemGetiCommsError();
		DebugPrint("%s Safety Error [%d],  SPI Error [%d], iComms Error [%d]", SFT_TAG, SafetyErrors, SPIErrors, iCommsErrors);

		static uint8_t s = 0;

		HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, s);
	        Init_GPIOs();


            if (SafetyErrors == 0 &&  SPIErrors == 0 &&  iCommsErrors == 0 ) {
                HAL_Delay(1000);
                Ring_0_Display_memory[3] = 0xFFFFFF;
                Send_Whole_Ring_from_Ring_Memory();
            } else {
                if (SafetyErrors >= 1) {
                    // SafetyError Flashes Red
                    // HAL_GPIO_WritePin(Safety_LED_GPIO_Port, Safety_LED_Pin, 1);
                    Ring_0_Display_memory[0] = 0xFF0000;
                    Send_Whole_Ring_from_Ring_Memory();
                    HAL_Delay(1000);
                }
                if (SPIErrors >= 1) {
                    Ring_0_Display_memory[1] = 0x00FF00;
                    Send_Whole_Ring_from_Ring_Memory();
                    HAL_Delay(1000);
                }

                if (iCommsErrors >= 1) {
                    Ring_0_Display_memory[2] = 0x0000FF;
                    Send_Whole_Ring_from_Ring_Memory();
                    HAL_Delay(1000);
                }
            }

        s = !s;
#endif
	}
}}