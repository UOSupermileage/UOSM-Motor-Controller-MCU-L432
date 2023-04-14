/*
 * CANMessageLookUpModule.c
 *
 *  Created on: Dec 4, 2022
 *      Author: mingy
 */

#include "DataAggregationModule.h"
#include "CANMessageLookUpModule.h"
#include "SerialDebugDriver.h"
#include "CANDriver.h"
#include "SafetyModule.h"

#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

// Callbacks
void ThrottleDataCallback(iCommsMessage_t* msg);
void ErrorDataCallback(iCommsMessage_t* msg);
void SpeedDataCallback(iCommsMessage_t* msg);
void EventDataCallback(iCommsMessage_t* msg);

/*********************************************************************************
 *
 * 		Look up table for CAN ID and meta data about its payload
 *
 **********************************************************************************/
const ICommsMessageInfo CANMessageLookUpTable[NUMBER_CAN_MESSAGE_IDS] =
	{
			// Message Index			CAN ID		Num of Bytes		Callback
			{THROTTLE_DATA_ID,			0x0001, 			2,		&ThrottleDataCallback},
			{SPEED_DATA_ID,				0x0002,				4,		&SpeedDataCallback},
			{EVENT_DATA_ID, 			0x0400,				2,		&EventDataCallback},
			{ERROR_DATA_ID,				0x0401,				2,		&ErrorDataCallback},
	};

void ThrottleDataCallback(iCommsMessage_t* msg)
{
	DebugPrint("ThrottleDataCallback! %d", msg->standardMessageID);
	// DebugPrint("Throttle Raw: [%x][%x] length: [%d]", msg.data[1], msg.data[0], msg.dataLength);
	uint32_t throttle = readMsg(msg);
	DebugPrint("CAN Throttle percentage received: %d", throttle);
	Safety_SetThrottlePercentage(throttle);
}
void ErrorDataCallback(iCommsMessage_t* msg)
{
//	DebugPrint("ErrorDataCallback! %d", msg->standardMessageID);
}
void SpeedDataCallback(iCommsMessage_t* msg)
{
//	DebugPrint("SpeedDataCallback! %d", msg->standardMessageID);
}
void EventDataCallback(iCommsMessage_t* msg)
{
//	DebugPrint("EventDataCallbackCallback! %d", msg->standardMessageID);
}

PUBLIC const ICommsMessageInfo* CANMessageLookUpGetInfo(ICommsMessageLookUpIndex id) {
	return &CANMessageLookUpTable[id];
}
