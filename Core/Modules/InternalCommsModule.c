/*
 * InternalCommsModule.c
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 *
 *  This is the module for processing information from the cars intercommunication system
 *  Can only import interface files
 */

#include "InternalCommsModule.h"
#include "SerialDebugDriver.h"
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)


/***********************************************************
 *
 * 	ICOMMS_DRIVER_INITIALIZE(...)
 *
 * 	This function initializes the car's internal communications
 *
 *	Arguments:
 *	None
 *
 * 	Returns:
 * 	- 1 if initialization successful
 * 	- 0 if initialization failed
 *
 ************************************************************/
#define ICOMMS_DRIVER_INITIALIZE(...) CANSPI_Initialize(__VA_ARGS__)

/***********************************************************
 *
 * 	ICOMMS_DRIVER_MESSAGE_AVAILABLE(...)
 *
 * 	This function checks whether there are messages
 *	ready to be read from the car's internal communications
 *
 *	Arguments:
 *	None
 *
 * 	Returns:
 * 	- 1 if message is available
 * 	- 0 if no message is available
 *
 ************************************************************/
#define ICOMMS_DRIVER_MESSAGE_AVAILABLE(...) CANSPI_messagesInBuffer(__VA_ARGS__)

/***********************************************************
 *
 * 	ICOMMS_DRIVER_TRANSMIT_MESSAGE(...)
 *
 * 	This function transmits a message over the car's communication
 *
 *	Arguments:
 *	uCAN_MSG pointer
 *
 * 	Returns:
 * 	- 1 if message was sent
 * 	- 0 if messages was not able to be sent
 *
 ************************************************************/
#define ICOMMS_DRIVER_TRANSMIT_MESSAGE(...) CANSPI_Transmit(__VA_ARGS__)

/***********************************************************
 *
 * 	ICOMMS_DRIVER_RECEIVE_MESSAGE(...)
 *
 * 	This function receives a message over the car's communication
 *
 *	Arguments:
 *	uCAN_MSG pointer
 *
 * 	Returns:
 * 	- 1 if message was retrieved
 * 	- 0 if no message was retrieved
 *
 ************************************************************/
#define ICOMMS_DRIVER_RECEIVE_MESSAGE(...) CANSPI_Receive(__VA_ARGS__)

// Circular Queue
#define QUEUE_MAX 32
typedef struct ICommsQueue_t
{
	uint32_t head;
	uint32_t tail;
	uint32_t count;
	iCommsMessage_t data[QUEUE_MAX];
}ICommsQueue_t;

ICommsQueue_t ICommsRxQueue;

PRIVATE void ICommsQueue_init( ICommsQueue_t * q );
PRIVATE void ICommsQueue_enqueue( ICommsQueue_t * q, iCommsMessage_t value );
PRIVATE iCommsMessage_t ICommsQueue_dequeue( ICommsQueue_t * q);


PUBLIC result_t IComms_Init()
{
	result_t ret = ICOMMS_DRIVER_INITIALIZE();
	ICommsQueue_init(&ICommsRxQueue);
	return ret;
}

PUBLIC result_t IComms_Transmit(iCommsMessage_t * txMsg)
{
	result_t ret = ICOMMS_DRIVER_TRANSMIT_MESSAGE(txMsg);
	return ret;
}
PUBLIC result_t IComms_ReceiveNextMessage(iCommsMessage_t * rxMsg)
{
	// if nothing to dequeue return fail
	if(ICommsRxQueue.count == 0) return RESULT_FAIL;
	// dequeue return ok
	*rxMsg = ICommsQueue_dequeue(&ICommsRxQueue);
	return RESULT_OK;
}


PUBLIC uint8_t IComms_HasRxMessage()
{
	if(ICommsRxQueue.count != 0) return 1;
	return 0;
}

// Since we are currently polling, the update function needs to be called.
// Alternatively this can be put into an interrupt
PUBLIC void IComms_Update()
{
	while (ICOMMS_DRIVER_MESSAGE_AVAILABLE() != 0)
	{
		iCommsMessage_t rxMsg;
		result_t ret = ICOMMS_DRIVER_RECEIVE_MESSAGE(&rxMsg);
		if(ret == RESULT_FAIL)
		{
			DebugPrint("#ICM: FAILED TO RETRIEVE ICOMMS MESSAGE FROM DRIVER");
		}
		else{
			DebugPrint("#ICM: MESSAGE RECIEVED, ADDING TO QUEUE");
			// enqueue
			ICommsQueue_enqueue(&ICommsRxQueue, rxMsg);
			DebugPrint("#ICM: Standard ID: %d", rxMsg.standardMessageID);
			DebugPrint("#ICM: DLC: %d", rxMsg.dataLength);
			for(uint8_t i=0; i<rxMsg.dataLength; i++) DebugPrint("#ICM: Data[%d]: %d", i, rxMsg.data[i]);
		}


	}
}

/*********************************************************************************
 *
 * 		Circular queue for icomms input
 *
 **********************************************************************************/

PRIVATE void ICommsQueue_init( ICommsQueue_t * q )
{
	q->head = 0;
	q->count = 0;
	q->tail = 0;
}

PRIVATE void ICommsQueue_enqueue( ICommsQueue_t * q, iCommsMessage_t value )
{
	if(q->count < QUEUE_MAX)
	{
		// copy message struct into the queue
		q->data[q->tail].standardMessageID = value.standardMessageID;
		q->data[q->tail].dataLength = value.dataLength;

		for(uint8_t i; i < value.dataLength; i++)
			q->data[q->tail].data[i] = value.data[i];

		// increment count
		q->count ++;
		q->tail = (q->tail + 1) % QUEUE_MAX; // circular
	}
}

PRIVATE iCommsMessage_t ICommsQueue_dequeue( ICommsQueue_t * q)
{
	if(q->count > 0)
	{
		iCommsMessage_t value;

		value = q->data[q->head];


		q->count --;
		q->head = (q->head + 1) % QUEUE_MAX;
		return value;
	}
	DebugPrint("ICM: Error dequeuing");
	return (iCommsMessage_t){0xFFFF,0,0};// garbage value, should never be returned
}


