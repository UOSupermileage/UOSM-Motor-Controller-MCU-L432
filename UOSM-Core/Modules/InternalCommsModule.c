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
#include "CANDriver.h"
#include "CANMessageLookUpModule.h"
#include <string.h>

const char *ICM_TAG = "#ICM:";

static const uint8_t batchSize = 5;

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

PUBLIC result_t IComms_Init() {
	result_t ret = ICOMMS_DRIVER_INITIALIZE();
	return ret;
}

PUBLIC result_t IComms_Transmit(iCommsMessage_t *txMsg) {
	result_t ret = ICOMMS_DRIVER_TRANSMIT_MESSAGE(txMsg);
	return ret;
}

PUBLIC void IComms_PeriodicReceive() {
	for (uint8_t i = 0; i < batchSize && ICOMMS_DRIVER_MESSAGE_AVAILABLE() != 0; i++) {
		// Create an empty message to populate
        iCommsMessage_t rxMsg;

		result_t ret = ICOMMS_DRIVER_RECEIVE_MESSAGE(&rxMsg);
		if (ret == RESULT_FAIL) {
			DebugPrint("#ICM: FAILED TO RETRIEVE ICOMMS MESSAGE FROM DRIVER");
		} else {
			uint8_t lookupTableIndex = 0;

			// Lookup CAN message in table
			// Exit while loop if message found or if end of table reached
			while (rxMsg.standardMessageID != CANMessageLookUpTable[lookupTableIndex].messageID && lookupTableIndex < NUMBER_CAN_MESSAGE_IDS) {
				// DebugPrint("%s msgId[%x] != [%x]", ICM_TAG, rxMsg.standardMessageID, CANMessageLookUpTable[lookupTableIndex].messageID);
				lookupTableIndex++;
			}

			// handle the case where the message is no recognized by the look up table
			if (lookupTableIndex < NUMBER_CAN_MESSAGE_IDS) {
				// DebugPrint("%s Executing callback", ICM_TAG);
                // Execute callback for message
				CANMessageLookUpTable[lookupTableIndex].canMessageCallback(&rxMsg);
			} else {
				DebugPrint("%s Unknown message id [%x], index [%d]", ICM_TAG, rxMsg.standardMessageID, lookupTableIndex);
			}
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

PUBLIC iCommsMessage_t IComms_CreateUint32BitMessage(uint16_t standardMessageID, uint32_t value) {
	uint8_t data[8];
	data[0] = value;
	data[1] = value >> 8;
	data[2] = value >> 16;
	data[3] = value >> 24;

	return IComms_CreateMessage(standardMessageID, 4, data);
}

PUBLIC iCommsMessage_t IComms_CreateInt32BitMessage(uint16_t standardMessageID, int32_t value) {
	uint8_t data[8];
	data[0] = value;
	data[1] = value >> 8;
	data[2] = value >> 16;
	data[3] = value >> 24;

	return IComms_CreateMessage(standardMessageID, 4, data);
}

PUBLIC iCommsMessage_t IComms_CreateErrorMessage(uint16_t standardMessageID, ErrorCode code, flag_status_t status) {
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

PUBLIC iCommsMessage_t IComms_CreatePairUInt16BitMessage(uint16_t standardMessageID, uint16_t a, uint16_t b) {
        uint8_t data[8];
        data[0] = a;
        data[1] = a >> 8;
        data[2] = b;
        data[3] = b >> 8;

        return IComms_CreateMessage(standardMessageID, 4, data);
}

PUBLIC uint16_pair_t readMsgPairUInt16Bit(iCommsMessage_t *msg) {
        uint16_pair_t pair = {};

        if (msg->dataLength != 4) { return pair; }

        pair.a = msg->data[1] << 8;
        pair.a |= msg->data[0];

        pair.b = msg->data[3] << 8;
        pair.b |= msg->data[2];

        return pair;
}