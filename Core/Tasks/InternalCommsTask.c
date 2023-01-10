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
#include "CANDriver.h"


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

	for(;;)
	{
		cycleTick += TIMER_INTERNAL_COMMS_TASK;
		osDelayUntil(cycleTick);

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
