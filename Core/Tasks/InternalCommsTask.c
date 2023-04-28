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

#define STACK_SIZE 128 * 8
#define INTERNAL_COMMS_TASK_PRIORITY (osPriority_t) osPriorityRealtime3
#define TIMER_INTERNAL_COMMS_TASK 100UL

#define THROTTLE_ERROR_BROADCAST_RATE 5
#define DEADMAN_BROADCAST_RATE 3
#define MOTOR_INIT_BROADCAST_RATE 3

PUBLIC void InitInternalCommsTask(void);
PRIVATE void InternalCommsTask(void *argument);

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
PRIVATE void InternalCommsTask(void *argument)
{
	uint32_t cycleTick = osKernelGetTickCount();
	DebugPrint("%s icomms", ICT_TAG);

	const ICommsMessageInfo *errorInfo = CANMessageLookUpGetInfo(ERROR_DATA_ID);
	const ICommsMessageInfo *eventInfo = CANMessageLookUpGetInfo(EVENT_DATA_ID);

	uint8_t throttleErrorBroadcastCounter = 0;
	uint8_t deadmanBroadcastCounter = 0;
	uint8_t motorInitCounter = 0;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

	IComms_Init();
	for (;;)
	{
		cycleTick += TIMER_INTERNAL_COMMS_TASK;
		osDelayUntil(cycleTick);

#ifdef PROFILE_ICOMMS
		IComms_PeriodicReceive();

		// Broadcast
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
#endif
	}
}
