/*
 * InternalCommsTask.c
 *
 *  Created on: Sep 10, 2022
 *      Author: mingy
 */

#include "InternalCommsTask.h"
#include "CANMessageLookUpModule.h"
#include "DataAggregationModule.h"
#include "InternalCommsModule.h"
#include "Profiles.h"
#include "BackupRegister.h"

#define STACK_SIZE 128 * 8
#define INTERNAL_COMMS_TASK_PRIORITY (osPriority_t) osPriorityRealtime3
#define TIMER_INTERNAL_COMMS_TASK 100UL

#define THROTTLE_ERROR_BROADCAST_RATE 5
#define DEADMAN_BROADCAST_RATE 3
#define MOTOR_INIT_BROADCAST_RATE 3
#define MOTOR_RPM_BROADCAST_RATE 3

// =======MOTOR TEMP=============
#define MOTOR_TEMP_BROADCAST_RATE 3
// ==============================
void InitInternalCommsTask(void);
_Noreturn void InternalCommsTask(void *argument);

const char ICT_TAG[] = "#ICT:";

osThreadId_t InternalCommsTaskHandle;
const osThreadAttr_t InternalCommsTask_attributes = {
    .name = "InternalCommunications",
    .stack_size = STACK_SIZE,
    .priority = INTERNAL_COMMS_TASK_PRIORITY,
};

void InitInternalCommsTask(void) { InternalCommsTaskHandle = osThreadNew(InternalCommsTask, NULL, &InternalCommsTask_attributes); }
_Noreturn void InternalCommsTask(void *argument) {
    // Pull CAN CS High
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

    uint32_t cycleTick = osKernelGetTickCount();
    DebugPrint("%s icomms", ICT_TAG);

    // Create counters to keep track of when to send messages

    // ======For motor temp========
    uint8_t motorTempBroadcastCounter = 0;
    // ============================

    uint8_t throttleErrorBroadcastCounter = 0;
    uint8_t deadmanBroadcastCounter = 0;
    uint8_t motorInitCounter = 0;
    uint8_t motorRPMBroadcastCounter = 0;

    if (Backup_GetFaultStatus() != Status_NoFault) {
        // TODO: Improve error reporting
        iCommsMessage_t errorMsg = IComms_CreateErrorMessage(0, Backup_GetFaultStatus(), 0, 0);
        IComms_Transmit(&errorMsg);
    }

    IComms_Init();
    for (;;) {
        cycleTick += TIMER_INTERNAL_COMMS_TASK;
        osDelayUntil(cycleTick);

#ifdef PROFILE_ICOMMS
        // Receive messages
        IComms_PeriodicReceive();

        // Broadcast messages
        if (throttleErrorBroadcastCounter >= THROTTLE_ERROR_BROADCAST_RATE) {
            DebugPrint("%s Sending Throttle Error Code %d!", ICT_TAG, SystemGetThrottleError());

            iCommsMessage_t throttleTxMsg = IComms_CreateEventMessage(THROTTLE_TOO_HIGH, SystemGetThrottleError());
            result_t r = IComms_Transmit(&throttleTxMsg);

            DebugPrint("%s Sending Throttle Error Code! [Result = %d]", ICT_TAG, r);
            throttleErrorBroadcastCounter = 0;
        } else {
            DebugPrint("Increment");
            throttleErrorBroadcastCounter++;
        }

        // Send Deadman signal to CANBUS to confirm system init
        if (deadmanBroadcastCounter == DEADMAN_BROADCAST_RATE) {
            iCommsMessage_t deadmanTxMsg = IComms_CreateEventMessage(DEADMAN, 1);
            IComms_Transmit(&deadmanTxMsg);

            deadmanBroadcastCounter = 0;
        } else {
            deadmanBroadcastCounter++;
        }

        if (motorInitCounter == MOTOR_INIT_BROADCAST_RATE) {
            iCommsMessage_t initTxMsg = IComms_CreateEventMessage(MOTOR_INITIALIZING, SystemGetMotorInitializing());
            IComms_Transmit(&initTxMsg);

            motorInitCounter = 0;
        } else {
            motorInitCounter++;
        }
        // =======MOTOR TEMP========
        if (motorTempBroadcastCounter == MOTOR_TEMP_BROADCAST_RATE) {
            iCommsMessage_t tmpTxMsg = IComms_CreateUint32BitMessage(MOTOR_TEMP_DATA_ID, SystemGetTemperature());
            IComms_Transmit(&tmpTxMsg);

            motorTempBroadcastCounter = 0;
        } else {
            motorTempBroadcastCounter++;
        }
        // ========================

        if (motorRPMBroadcastCounter == MOTOR_RPM_BROADCAST_RATE) {
            iCommsMessage_t rpmTxMsg = IComms_CreateInt32BitMessage(MOTOR_RPM_DATA_ID, SystemGetMotorVelocity());
            IComms_Transmit(&rpmTxMsg);

            motorRPMBroadcastCounter = 0;
        } else {
            motorRPMBroadcastCounter++;
        }
#endif
    }
}
