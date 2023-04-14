/*
 * InternalCommsModule.c
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 *
 *  This is the module for processing information from the cars intercommunication system
 *  Can only import interface files
 */
#include "CANDriver.h"
#include "InternalCommsModule.h"
#include "SerialDebugDriver.h"
#include <string.h>
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

typedef struct HeapNode
{
    uint16_t key;
    iCommsMessage_t value;
} HeapNode_t;

#define MAX_HEAP_SIZE 32
typedef struct heap
{
    uint8_t heapsize;
    HeapNode_t heapArray[MAX_HEAP_SIZE];
} Heap_t;

PUBLIC void Heap_InitHeap(Heap_t * h);
PUBLIC void Heap_Add(Heap_t * h, HeapNode_t n);
PUBLIC HeapNode_t Heap_RemoveMin(Heap_t* h);
PRIVATE void HeapifyMin(Heap_t* h, uint8_t parent);
PRIVATE void Swap(HeapNode_t* a, HeapNode_t* b);
uint8_t Heap_GetHeapSize(Heap_t * h);

Heap_t canRxHeap;

PUBLIC result_t IComms_Init()
{
	result_t ret = ICOMMS_DRIVER_INITIALIZE();
	Heap_InitHeap(&canRxHeap);
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
	if(Heap_GetHeapSize(&canRxHeap) == 0) return RESULT_FAIL;
	// dequeue return ok
	HeapNode_t n = Heap_RemoveMin(&canRxHeap);
	memcpy(rxMsg, &n.value, sizeof(iCommsMessage_t));
	return RESULT_OK;
}


PUBLIC uint8_t IComms_HasRxMessage()
{
	if(Heap_GetHeapSize(&canRxHeap) != 0) return 1;

	DebugPrint("No Messages...");
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
			HeapNode_t n = (HeapNode_t){rxMsg.standardMessageID, rxMsg};
			Heap_Add(&canRxHeap, n);
			DebugPrint("#ICM: Standard ID: %d", rxMsg.standardMessageID);
			DebugPrint("#ICM: DLC: %d", rxMsg.dataLength);
			for(uint8_t i=0; i<rxMsg.dataLength; i++) DebugPrint("#ICM: Data[%d]: %d", i, rxMsg.data[i]);
		}


	}
}

PUBLIC iCommsMessage_t IComms_CreateMessage(uint16_t standardMessageID, uint8_t dataLength, uint8_t data[8]) {
	iCommsMessage_t msg;
	msg.standardMessageID = standardMessageID;
	msg.dataLength = dataLength;

	memcpy(msg.data, data, 8);

	return msg;
}

PUBLIC iCommsMessage_t IComms_CreatePercentageMessage(uint16_t standardMessageID, percentage_t percentage) {
	uint8_t data[8];
	data[0] = percentage;
	data[1] = percentage >> 8;

	return IComms_CreateMessage(standardMessageID, 2, data);
}

PUBLIC iCommsMessage_t IComms_CreateErrorMessage(uint16_t standardMessageID, uint8_t code, uint8_t status) {
	uint8_t data[8];
	data[0] = status;
	data[1] = code;

	return IComms_CreateMessage(standardMessageID, 2, data);
}

PUBLIC iCommsMessage_t IComms_CreateEventMessage(uint16_t standardMessageID, uint8_t code, uint8_t status) {
	uint8_t data[8];
	data[0] = status;
	data[1] = code;

	return IComms_CreateMessage(standardMessageID, 2, data);
}

/*********************************************************************************
 *
 * 		Heap for Priority Queue
 *
 **********************************************************************************/
void Heap_InitHeap(Heap_t* h)
{
    h->heapsize = 0;
}

void Swap(HeapNode_t * a, HeapNode_t * b)
{
    HeapNode_t temp;
    memcpy(&temp, b, sizeof(HeapNode_t));
    memcpy(b, a, sizeof(HeapNode_t));
    memcpy(a, &temp, sizeof(HeapNode_t));

}

void HeapifyMin(Heap_t* h, uint8_t parent)
{
    if (h->heapsize == 1) return;
    else
    {
        int smallest = parent;
        int l = 2 * parent + 1;
        int r = 2 * parent + 2;
        if (l < h->heapsize && h->heapArray[l].key < h->heapArray[smallest].key)
            smallest = l;
        if (r < h->heapsize && h->heapArray[r].key < h->heapArray[smallest].key)
            smallest = r;
        if (smallest != parent)
        {
            Swap(&(h->heapArray[parent]), &(h->heapArray[smallest]));
            HeapifyMin(h, smallest);
        }
    }
}
void Heap_Add(Heap_t* h, HeapNode_t n)
{
    if (h->heapsize == MAX_HEAP_SIZE) return;
    if (h->heapsize == 0)
    {
        memcpy(&(h->heapArray[0]), &n, sizeof(HeapNode_t));
        h->heapsize += 1;
    }
    else
    {
        memcpy(&(h->heapArray[h->heapsize]), &n, sizeof(HeapNode_t));
        h->heapsize += 1;
        for (int i = h->heapsize / 2 - 1; i >= 0; i--)
        {
            HeapifyMin(h, i);
        }
    }
}
HeapNode_t Heap_RemoveMin(Heap_t* h)
{
    if (h->heapsize == 0) return (HeapNode_t) { 0xFFFF, (iCommsMessage_t){0xFFFF,0xFF,NULL}};
    HeapNode_t min;
    memcpy(&min, &(h->heapArray[0]), sizeof(HeapNode_t));

    Swap(&(h->heapArray[0]), &(h->heapArray[h->heapsize - 1]));
    h->heapsize -= 1;
    for (int i = h->heapsize / 2 - 1; i >= 0; i--)
    {
        HeapifyMin(h, i);
    }
    return min;
}

uint8_t Heap_GetHeapSize(Heap_t * h)
{
	return h->heapsize;
}
