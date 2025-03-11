//
// Created by David on 2025-01-28.
//

#include "DataAggregationModule.h"
#include "SafetyTask.h"

#include "stdint.h"
#include "TempTask.h"
#include "TemperatureSensor.h"

//#include "LEDStatusDriver.h"
#include "Profiles.h"

#define STACK_SIZE 128*8
#define TEMP_TASK_PRIORITY (osPriority_t) osPriorityRealtime5
#define TIMER_TEMP_TASK 1000UL

const char TEMP_TAG[] = "TEMP:";

void InitTempTask(void);

osThreadId_t TempTaskHandle;

const osThreadAttr_t TempTask_attributes = {
    .name = "TempTask",
    .stack_size = STACK_SIZE,
    .priority = TEMP_TASK_PRIORITY,
};

void TempTask();

void InitTempTask(void){
    TempTaskHandle = osThreadNew(TempTask, NULL, &TempTask_attributes);
}

void TempTask(){
    uint32_t cycleTick = osKernelGetTickCount();
    DebugPrint("%s Temp", TEMP_TAG);

    while (TemperatureInit() != RESULT_OK) {
        DebugPrint("Failed to init temp sensor.");
        cycleTick += 200;
        osDelayUntil(cycleTick);
    }

    for(;;){

        cycleTick += TIMER_TEMP_TASK;
        osDelayUntil(cycleTick);
     #ifdef PROFILE_SAFETY

        uint16_t temp;
        TemperatureGetTemp(TEMP_TOP, &temp);

        SystemSetTemperature(temp);

        DebugPrint("%s TEMP [%d]", TEMP_TAG, SystemGetTemperature());

    #endif
    }
}


