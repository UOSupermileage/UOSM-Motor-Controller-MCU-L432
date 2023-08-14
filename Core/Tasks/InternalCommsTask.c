/*
 * InternalCommsTask.c
 *
 *  Created on: Sep 10, 2022
 *      Author: mingy
 */

// #include "CANMessageLookUpModule.h"
#include "InternalCommsTask.h"
#include "InternalCommsModule.h"
#include "CANMessageLookUpModule.h"
#include "Profiles.h"
#include "DataAggregationModule.h"
#include "SPIMotorDriver.h"

#define STACK_SIZE 128 * 8
#define INTERNAL_COMMS_TASK_PRIORITY (osPriority_t) osPriorityRealtime3
#define TIMER_INTERNAL_COMMS_TASK 100UL

#define THROTTLE_ERROR_BROADCAST_RATE 5
#define DEADMAN_BROADCAST_RATE 3
#define MOTOR_INIT_BROADCAST_RATE 3
#define MOTOR_RPM_BROADCAST_RATE 3

PUBLIC void InitInternalCommsTask(void);
PRIVATE _Noreturn void InternalCommsTask(void *argument);

const char ICT_TAG[] = "#ICT:";

osThreadId_t InternalCommsTaskHandle;
const osThreadAttr_t InternalCommsTask_attributes = {
	.name = "InternalCommunications",
	.stack_size = STACK_SIZE,
	.priority = INTERNAL_COMMS_TASK_PRIORITY,
};

PUBLIC void InitInternalCommsTask(void)
{
	InternalCommsTaskHandle = osThreadNew(InternalCommsTask, NULL, &InternalCommsTask_attributes);
}
PRIVATE _Noreturn void InternalCommsTask(void *argument)
{
	uint32_t cycleTick = osKernelGetTickCount();
	DebugPrint("%s icomms", ICT_TAG);

        // Get CAN message info to be able to send these types of messages
	const ICommsMessageInfo *errorInfo = CANMessageLookUpGetInfo(ERROR_DATA_ID);
	const ICommsMessageInfo *eventInfo = CANMessageLookUpGetInfo(EVENT_DATA_ID);
	const ICommsMessageInfo *rpmInfo = CANMessageLookUpGetInfo(MOTOR_RPM_DATA_ID);

        // Create counters to keep track of when to send messages
	uint8_t throttleErrorBroadcastCounter = 0;
	uint8_t deadmanBroadcastCounter = 0;
	uint8_t motorInitCounter = 0;
	uint8_t motorRPMBroadcastCounter = 0;

        // Pull CAN CS High
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

	IComms_Init();
	for (;;)
	{
		cycleTick += TIMER_INTERNAL_COMMS_TASK;
		osDelayUntil(cycleTick);

#ifdef PROFILE_ICOMMS
                // Receive messages
		IComms_PeriodicReceive();

		// Broadcast messages
		if (throttleErrorBroadcastCounter >= THROTTLE_ERROR_BROADCAST_RATE)
		{
			DebugPrint("%s Sending Throttle Error Code %d!", ICT_TAG, SystemGetThrottleError());

			iCommsMessage_t throttleTxMsg = IComms_CreateErrorMessage(errorInfo->messageID, THROTTLE_TOO_HIGH, SystemGetThrottleError());
			result_t r = IComms_Transmit(&throttleTxMsg);

			DebugPrint("%s Sending Throttle Error Code! [Result = %d]", ICT_TAG, r);
			throttleErrorBroadcastCounter = 0;
		}
		else
		{
			DebugPrint("Increment");
			throttleErrorBroadcastCounter++;
		}

		// Send Deadman signal to CANBUS to confirm system init
		if (deadmanBroadcastCounter == DEADMAN_BROADCAST_RATE)
		{
			iCommsMessage_t deadmanTxMsg = IComms_CreateEventMessage(eventInfo->messageID, DEADMAN, 1);
			IComms_Transmit(&deadmanTxMsg);

			deadmanBroadcastCounter = 0;
		} else {
			deadmanBroadcastCounter++;
		}


		if (motorInitCounter == MOTOR_INIT_BROADCAST_RATE)
		{
			iCommsMessage_t initTxMsg = IComms_CreateEventMessage(eventInfo->messageID, MOTOR_INITIALIZING, SystemGetMotorInitializing());
			IComms_Transmit(&initTxMsg);

			motorInitCounter = 0;
		} else {
			motorInitCounter++;
		}

		if (motorRPMBroadcastCounter == MOTOR_RPM_BROADCAST_RATE) {
			iCommsMessage_t rpmTxMsg = IComms_CreateInt32BitMessage(rpmInfo->messageID, SystemGetMotorVelocity());
			IComms_Transmit(&rpmTxMsg);

			motorRPMBroadcastCounter = 0;
		} else {
			motorRPMBroadcastCounter++;
		}
#endif
	}
}
