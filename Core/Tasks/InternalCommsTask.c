/*
 * InternalCommsTask.c
 *
 *  Created on: Sep 10, 2022
 *      Author: mingy
 */

#include "InternalCommsModule.h"
#include "InternalCommsTask.h"
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
	DebugPrint("icomms");
	ICommsInit();

	uint32_t counter = 0;

	for(;;)
	{
		cycleTick += TIMER_INTERNAL_COMMS_TASK;
		osDelayUntil(cycleTick);

		counter++;

		if (counter == 100) {
			sendThrottlePercentage(5);
		}else if (counter == 200) {
			sendThrottlePercentage(80);
		} else if (counter == 300) {
			sendThrottlePercentage(50);
		} else if (counter == 400) {
			sendThrottlePercentage(0);
			counter = 0;
		}


		if(ICommsMessageAvailable() > 0)
		{
			iCommsMessage_t rxMsg;

			ICommsReceive(&rxMsg);
			DebugPrint("Standard ID: %d", rxMsg.standardMessageID);
			DebugPrint("DLC: %d", rxMsg.dataLength);
			for(uint8_t i=0; i<rxMsg.dataLength; i++) DebugPrint("Data[%d]: %d", i, rxMsg.data[i]);

			switch (rxMsg.standardMessageID) {
			case CAN_THROTTLE:;
				uint32_t torque = readMsg(&rxMsg);
				DebugPrint("CAN Throttle percentage received: %d%", torque);

				datastoreSetTargetTorquePercentage(torque);
			default:
				DebugPrint("Unknown CAN message with id [%d] received!", rxMsg.standardMessageID);
			}
		}
	}
}

PRIVATE result_t sendThrottlePercentage(const uint8_t percentage) {

	iCommsMessage_t txMsg;
	txMsg.standardMessageID = CAN_THROTTLE;
	txMsg.dataLength = 1;

	if (percentage > 100) {
		txMsg.data[0] = 100;
	} else if (percentage < -100) {
		txMsg.data[0] = -100;
	} else {
		txMsg.data[0] = percentage;
	}

	// Temporarily here to bypass CAN
	datastoreSetTargetTorquePercentage(percentage);
	return ICommsTransmit(&txMsg);
}
