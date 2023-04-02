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
#include "SerialDebugDriver.h"
#include "tmc/ic/TMC4671/TMC4671.h"
#include "tmc/ic/TMC6200/TMC6200.h"
#include "tmc/ic/TMC6200/TMC6200_Register.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128 * 4
#define MOTOR_TASK_PRIORITY (osPriority_t) osPriorityRealtime1
#define TIMER_MOTOR_TASK 500UL
#define TIMER_MOTOR_REINIT_DELAY 5000UL

const char MOT_TAG[] = "#MOT:";

PUBLIC void InitMotorTask(void);
PRIVATE void MotorTask(void *argument);

osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
    .name = "MotorTask",
    .stack_size = STACK_SIZE,
    .priority = MOTOR_TASK_PRIORITY,
};

PUBLIC void InitMotorTask(void) {
#ifdef MOTOR_PROFILE
    MotorTaskHandle = osThreadNew(MotorTask, NULL, &MotorTask_attributes);
#endif
}
/// task
PRIVATE void MotorTask(void *argument) {
    uint32_t motorInitialized = 0;

    uint32_t cycleTick = osKernelGetTickCount();
    DebugPrint("%s Initializing MotorTask", MOT_TAG);

#if MOTOR_MODE == 0 || MOTOR_MODE == 1
    motorInitialized = MotorInit();
#elif MOTOR_MODE == 2
    MotorSetCS(TMC4671_CS, GPIO_PIN_SET);
    MotorSetCS(TMC6200_CS, GPIO_PIN_SET);
    MotorEnableDriver(ENABLED);
    HAL_Delay(500);

#ifdef MOTOR_CLEAR_CHARGE_PUMP_FAULT
    MotorClearChargePump();
#endif
#endif

    for (;;) {
        cycleTick += TIMER_MOTOR_TASK;
        osDelayUntil(cycleTick);

#ifdef MOTOR_FIXED_THROTTLE
        SystemSetThrottlePercentage(MOTOR_FIXED_THROTTLE);
#endif

        if (motorInitialized) {
#if MOTOR_MODE == 0
#if MOTOR_CONFIG_MODE_RAMP_MODE_MOTION == 1
            DebugPrint("%s Target Torque [%x]", MOT_TAG,
                       SystemGetThrottlePercentage() *
                           MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS / 1000);
            MotorRotateTorque(SystemGetThrottlePercentage() *
                              MOTOR_CONFIG_PID_TORQUE_FLUX_LIMITS / 1000);
#elif MOTOR_CONFIG_MODE_RAMP_MODE_MOTION == 2
            DebugPrint("%s Target Velocity [%x]", MOT_TAG,
                       MOTOR_FIXED_THROTTLE);
            MotorRotate(SystemGetTargetVelocity());
#endif
#endif

#if MOTOR_MODE == 0 || MOTOR_MODE == 1
            MotorPeriodicJob(cycleTick);
#endif
        } else {
            SystemSetSPIError(Set);
            DebugPrint("%s Failed to initialize motor!", MOT_TAG);

            // If motor failed to initialize, wait and then reinit
            osDelay(TIMER_MOTOR_REINIT_DELAY);
            motorInitialized = MotorInit();
        }

#ifdef VERBOSE
        MotorPrintFaults();
#endif
    }
/// task
}
