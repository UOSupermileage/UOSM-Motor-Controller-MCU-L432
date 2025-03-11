/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#include "MotorTask.h"
#include "DataAggregationModule.h"
#include "MotorParameters.h"
#include "Profiles.h"
#include "tmc/ic/TMC4671/TMC4671.h"

// TODO: Does this task need more memory?
#define STACK_SIZE 128 * 8
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityRealtime
#define TIMER_MOTOR_TASK 150UL

const char MOT_TAG[] = "#MOT:";

void InitMotorTask(void);
void MotorTask(void *argument);

osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
	.name = "MotorTask",
	.stack_size = STACK_SIZE,
	.priority = MOTOR_TASK_PRIORITY,
};

void InitMotorTask(void)
{
#ifdef PROFILE_MOTOR
	MotorTaskHandle = osThreadNew(MotorTask, NULL, &MotorTask_attributes);
#endif
}
/// [task]
/**
 * Execution Loop for Motor Task.
 */
void MotorTask(void *argument)
{
	// Keep track of current tick count
	uint32_t cycleTick = osKernelGetTickCount();
        
	// Initialize the system as configured in MotorParameters.h
	DebugPrint("%s Initializing MotorTask", MOT_TAG);
	uint8_t motorInitialized = MotorInit();

	for (;;) {
            // Increment cycleTick and wait until the delay has passed
            cycleTick += TIMER_MOTOR_TASK;
            osDelayUntil(cycleTick);

#if MOTOR_MODE == 0 || MOTOR_MODE == 1
            // Store Motor RPM from TMC in the Aggregator
            SystemSetMotorVelocity(MotorGetActualVelocity());

            // Enable 6200 depending on state stored in Aggregator
//            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, SystemGetDriverEnabled() == Set ? GPIO_PIN_SET : GPIO_PIN_RESET);

            if (motorInitialized) {
#if MOTOR_MODE == 0
                velocity_t v = (MAX_VELOCITY / MAX_PERCENTAGE) * SystemGetThrottlePercentage();

                DebugPrint("%s Target Velocity [%d RPM]", MOT_TAG, v);
                MotorRotateVelocity(v);

                MotorPeriodicJob();
#endif
            } else {
                // Motor was not initialized. This indicates that communication with the TMC4671 or TMC6200 failed.
                SystemSetSPIError(Set);
                DebugPrint("%s Failed to initialize motor!", MOT_TAG);

                // Motor failed to initialize, wait and then reinit
                motorInitialized = MotorInit();
            }

            // Read registers in TMC6200 and check for faults
            MotorPrintFaults();
#endif
	}
	/// [task]
}
