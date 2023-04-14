/*
 * CANMessageLookUpModule.h
 *
 *  Created on: Dec 4, 2022
 *      Author: mingy
 */

#ifndef MODULES_CANMESSAGELOOKUPMODULE_H_
#define MODULES_CANMESSAGELOOKUPMODULE_H_

#include "ApplicationTypes.h"


// CAN Message IDs for each piece of data
// Details can be found on the CAN Reference Document
#define NUMBER_CAN_MESSAGE_IDS 4

typedef enum ICommsMessageLookUpIndex
{
	THROTTLE_DATA_ID,
	SPEED_DATA_ID,
	EVENT_DATA_ID,
	ERROR_DATA_ID,
} ICommsMessageLookUpIndex;

typedef uint16_t MessageCANID_t;

typedef struct
{
	ICommsMessageLookUpIndex index;
	MessageCANID_t messageID;
	uint8_t numberOfBytes;
	void (*canMessageCallback)(iCommsMessage_t* msg);

} ICommsMessageInfo;

extern const ICommsMessageInfo CANMessageLookUpTable[NUMBER_CAN_MESSAGE_IDS];

PUBLIC const ICommsMessageInfo* CANMessageLookUpGetInfo(ICommsMessageLookUpIndex id);

#endif /* MODULES_CANMESSAGELOOKUPMODULE_H_ */
