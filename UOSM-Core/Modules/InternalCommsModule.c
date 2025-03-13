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

result_t IComms_Init() {
    result_t ret = ICOMMS_DRIVER_INITIALIZE();
    return ret;
}

result_t IComms_Transmit(iCommsMessage_t* txMsg) {
    result_t ret = ICOMMS_DRIVER_TRANSMIT_MESSAGE(txMsg);
    return ret;
}

void IComms_PeriodicReceive() {
    for (uint8_t i = 0; i < batchSize && ICOMMS_DRIVER_MESSAGE_AVAILABLE() != 0; i++) {
        // Create an empty message to populate
        iCommsMessage_t rxMsg;

        if (ICOMMS_DRIVER_RECEIVE_MESSAGE(&rxMsg) == RESULT_OK) {
            if (rxMsg.standardMessageID >= NUMBER_CAN_MESSAGE_IDS) {
                DebugPrint("CAN: Unknown message id [%x]", rxMsg.standardMessageID);
                return;
            }

            // Execute callback for message
            if (rxMsg.dataLength != CANMessageLookUpTable[rxMsg.standardMessageID].numberOfBytes) {
                DebugPrint("CAN: message id [%x] has incorrect length of [%d]", rxMsg.standardMessageID, rxMsg.dataLength);
                return;
            }

            CANMessageLookUpTable[rxMsg.standardMessageID].canMessageCallback(&rxMsg);
        } else {
            DebugPrint("#ICM: FAILED TO RETRIEVE ICOMMS MESSAGE FROM DRIVER");
        }
    }
}

iCommsMessage_t IComms_CreateMessage(uint16_t standardMessageID, uint8_t dataLength, uint8_t data[8]) {
    iCommsMessage_t msg;
    msg.standardMessageID = standardMessageID;
    msg.dataLength = dataLength;

    memcpy(msg.data, data, 8);

    return msg;
}

iCommsMessage_t IComms_CreatePercentageMessage(uint16_t standardMessageID, percentage_t percentage) {
    uint8_t data[8];
    data[0] = percentage;
    data[1] = percentage >> 8;

    return IComms_CreateMessage(standardMessageID, 2, data);
}

iCommsMessage_t IComms_CreateUint32BitMessage(uint16_t standardMessageID, uint32_t value) {
    uint8_t data[8];
    data[0] = value;
    data[1] = value >> 8;
    data[2] = value >> 16;
    data[3] = value >> 24;

    return IComms_CreateMessage(standardMessageID, 4, data);
}

iCommsMessage_t IComms_CreateInt32BitMessage(uint16_t standardMessageID, int32_t value) {
    uint8_t data[8];
    data[0] = value;
    data[1] = value >> 8;
    data[2] = value >> 16;
    data[3] = value >> 24;

    return IComms_CreateMessage(standardMessageID, 4, data);
}

iCommsMessage_t IComms_CreateErrorMessage(uint8_t device, uint8_t code, uint32_t flags, uint32_t data) {
    uint8_t payload[8];

    // Device ID is bits 0..3
    payload[0] = device << 4;

    // Error Code is bits 4..7
    payload[0] |= code & 0x0F;

    // Flags is bits 8..31
    for (uint8_t i = 0; i < 3; i++)
        payload[1 + i] = (flags >> (8 * (2-i)));

    // Data is bits 31..63
    for (uint8_t i = 0; i < 4; i++)
        payload[4 + i] = (data >> (8 * (3-i)));

    return IComms_CreateMessage(ERROR_DATA_ID, 8, payload);
}

iCommsMessage_t IComms_CreateEventMessage(uint8_t code, uint8_t status) {
    uint8_t data[8];
    data[0] = status;
    data[1] = code;

    return IComms_CreateMessage(EVENT_DATA_ID, 2, data);
}



iCommsMessage_t IComms_CreateLightsMessage(lights_status_t lights) {
    uint8_t data[8];
    for (uint8_t i = 0; i < 4; i++)
        data[i] = lights.all >> (8 * (3-i));

    return IComms_CreateMessage(LIGHT_DATA_ID, 2, data);
}

iCommsMessage_t IComms_CreatePairInt32Message(uint16_t standardMessageID, int32_t a, int32_t b) {
    uint8_t data[8];

    for (uint8_t i = 0; i < 4; i++)
        data[i] = a >> (8 * (3-i));

    for (uint8_t i = 0; i < 4; i++)
        data[4 + i] = b >> (8 * (3-i));

    return IComms_CreateMessage(standardMessageID, 8, data);
}

result_t IComms_ReadPairInt32Message(iCommsMessage_t* msg, int32_t* a, int32_t* b) {
    if (msg->dataLength != 8) {
        return RESULT_FAIL;
    }

    *a = * (int32_t*) msg->data;
    *b = * (int32_t*) (msg->data + 4);

    return RESULT_OK;
}

iCommsMessage_t IComms_CreatePressureTemperatureMessage(pressure_t a, temperature_t b) {
    return IComms_CreatePairInt32Message(PRESSURE_TEMPERATURE_DATA_ID, a, b);
}
result_t IComms_ReadPressureTemperatureMessage(iCommsMessage_t* msg, pressure_t* a, temperature_t* b) {
    return IComms_ReadPairInt32Message(msg, a, b);
}

uint16_pair_t readMsgPairUInt16Bit(iCommsMessage_t* msg) {
    uint16_pair_t pair = {};

    if (msg->dataLength != 4) { return pair; }

    pair.a = msg->data[0] << 8;
    pair.a |= msg->data[1];

    pair.b = msg->data[2] << 8;
    pair.b |= msg->data[3];
    return pair;
}

iCommsMessage_t IComms_CreateEfficiencyMessage(lap_efficiencies_t* efficiencies) {
    uint8_t data[8];
    data[0] = efficiencies->lap_0;
    data[1] = efficiencies->lap_1;
    data[2] = efficiencies->lap_2;
    data[3] = efficiencies->lap_3;

    return IComms_CreateMessage(EFFICIENCY_DATA_ID, 2, data);
}

result_t IComms_ReadEfficiencyMessage(iCommsMessage_t* msg, lap_efficiencies_t* result) {
    result->lap_0 = msg->data[0];
    result->lap_1 = msg->data[1];
    result->lap_2 = msg->data[2];
    result->lap_3 = msg->data[3];

    return RESULT_OK;
}