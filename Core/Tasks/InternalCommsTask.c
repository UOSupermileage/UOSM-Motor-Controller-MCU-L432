/*
 * InternalCommsTask.c
 *
 *  Created on: Sep 10, 2022
 *      Author: mingy
 */


//#include "CANMessageLookUpModule.h"
#include "InternalCommsTask.h"
#include "InternalCommsModule.h"
#include "CANMessageLookUpModule.h"
#include "SerialDebugDriver.h"
#include "Profiles.h"
#include "DataAggregationModule.h"

// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*8
#define INTERNAL_COMMS_TASK_PRIORITY (osPriority_t) osPriorityRealtime3
#define TIMER_INTERNAL_COMMS_TASK 100UL

#define THROTTLE_ERROR_BROADCAST_RATE 5
#define DEADMAN_BROADCAST_RATE 2

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

	const ICommsMessageInfo* errorInfo = CANMessageLookUpGetInfo(ERROR_DATA_ID);
	const ICommsMessageInfo* eventInfo = CANMessageLookUpGetInfo(EVENT_DATA_ID);

	uint8_t throttleErrorBroadcastCounter = 0;
	uint8_t deadmanBroadcastCounter = 0;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

	IComms_Init();
	for(;;)
	{
		cycleTick += TIMER_INTERNAL_COMMS_TASK;
		osDelayUntil(cycleTick);

#ifdef PROFILE_ICOMMS
		IComms_Update();
		while(IComms_HasRxMessage())
		{
			iCommsMessage_t rxMsg;
			result_t ret = IComms_ReceiveNextMessage(&rxMsg);
			if(ret == RESULT_FAIL)
			{
				DebugPrint("%s Error Retrieving next message", ICT_TAG);
			}
			else{
				DebugPrint("%s Standard ID: %d", ICT_TAG, rxMsg.standardMessageID);
				DebugPrint("%s DLC: %d", ICT_TAG, rxMsg.dataLength);

				// for(uint8_t i=0; i<rxMsg.dataLength; i++) DebugPrint("%s Data[%d]: %d", ICT_TAG, i, rxMsg.data[i]);

				uint16_t lookupTableIndex = 0;

				// NOTE: with the current polling, new messages incoming while processing this batch of messages will not be processed until the next cycle.
				// lookup can message in table
				// Exit if message found or if end of table reached
				while(rxMsg.standardMessageID != CANMessageLookUpTable[lookupTableIndex].messageID && lookupTableIndex < NUMBER_CAN_MESSAGE_IDS)
				{
					// DebugPrint("%s msgId[%x] != [%x]", ICT_TAG, rxMsg.standardMessageID, CANMessageLookUpTable[lookupTableIndex].messageID);
					lookupTableIndex++;
				}

				// handle the case where the message is no recognized by the look up table
				if(lookupTableIndex < NUMBER_CAN_MESSAGE_IDS)
				{
					// DebugPrint("%s Executing callback", ICT_TAG);
					CANMessageLookUpTable[lookupTableIndex].canMessageCallback(&rxMsg);
				} else {
					DebugPrint("%s Unknown message id [%x], index [%d]", ICT_TAG, rxMsg.standardMessageID, lookupTableIndex);
				}
			}

		}

		// Broadcast
		if (throttleErrorBroadcastCounter >= THROTTLE_ERROR_BROADCAST_RATE) {

			DebugPrint("%s Sending Throttle Error Code %d!", ICT_TAG, SystemGetThrottleError());

			iCommsMessage_t throttleTxMsg = IComms_CreateErrorMessage(errorInfo->messageID, THROTTLE_TOO_HIGH, SystemGetThrottleError());
			result_t r = IComms_Transmit(&throttleTxMsg);

			DebugPrint("%s Sending Throttle Error Code! [Result = %d]", ICT_TAG, r);
			throttleErrorBroadcastCounter = 0;
		} else {
			DebugPrint("Increment");
			throttleErrorBroadcastCounter++;
		}

		// Send Deadman signal to CANBUS to confirm system init
		if (deadmanBroadcastCounter == DEADMAN_BROADCAST_RATE) {
			iCommsMessage_t deadmanTxMsg = IComms_CreateEventMessage(eventInfo->messageID, DEADMAN, 1);
			IComms_Transmit(&deadmanTxMsg);

			deadmanBroadcastCounter = 0;
		}
#endif
	}
}
