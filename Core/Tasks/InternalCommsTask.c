/*
 * InternalCommsTask.c
 *
 *  Created on: Sep 10, 2022
 *      Author: mingy
 */

#include "InternalCommsTask.h"

#include "InternalCommsModule.h"
#include "SerialDebugDriver.h"
#include "DatastoreModule.h"


// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*8
#define INTERNAL_COMMS_TASK_PRIORITY (osPriority_t) osPriorityRealtime
#define TIMER_INTERNAL_COMMS_TASK 50UL

PUBLIC void InitInternalCommsTask(void);
PRIVATE void InternalCommsTask(void *argument);

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
	DebugPrint("icomms setup");
	IComms_Init();

	uint32_t counter = 0;

	for(;;)
	{
		cycleTick += TIMER_INTERNAL_COMMS_TASK;
		osDelayUntil(cycleTick);

//		counter++;
//
//		if (counter <= 100) {
//			DebugPrint("Send throttle!");
//			// Set throttle 20%
//			iCommsMessage_t txMsg = createMsg(CAN_THROTTLE, 200);
//			result_t r = IComms_Transmit(&txMsg);
//
//			if (r == RESULT_OK) {
//				DebugPrint("OK");
//			} else {
//				DebugPrint("Failed to send");
//			}
//		} else if (counter <= 200) {
//			// Set throttle to 80%
//			iCommsMessage_t txMsg = createMsg(CAN_THROTTLE, 800);
//			IComms_Transmit(&txMsg);
//		} else {
//			counter = 0;
//
//			// Set throttle to 0%
//			iCommsMessage_t txMsg = createMsg(CAN_THROTTLE, 0);
//			IComms_Transmit(&txMsg);
//		}


		IComms_Update();
		while (IComms_HasRxMessage()) {
			iCommsMessage_t rxMsg;
			result_t ret = IComms_ReceiveNextMessage(&rxMsg);

			if(ret == RESULT_FAIL)
			{
				DebugPrint("#ICT: Error Retrieving next message");
			} else{
				DebugPrint("Standard ID: %d", rxMsg.standardMessageID);
				DebugPrint("DLC: %d", rxMsg.dataLength);
				for(uint8_t i=0; i<rxMsg.dataLength; i++) {
					DebugPrint("Data[%d]: %d", i, rxMsg.data[i]);
				}

				switch (rxMsg.standardMessageID) {
				case CAN_THROTTLE:
					;
					uint32_t throttle = readMsg(&rxMsg);
					DebugPrint("CAN Throttle percentage received: %d%", throttle);
					datastoreSetThrottlePercentage(throttle);
					break;
				default:
					DebugPrint("Unknown CAN message with id [%d] received!", rxMsg.standardMessageID);
					break;
				}
			}
		}
	}
}
