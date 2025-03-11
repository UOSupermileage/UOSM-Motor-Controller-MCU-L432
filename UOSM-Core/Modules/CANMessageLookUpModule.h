/*
 * CANMessageLookUpModule.h
 *
 *  Created on: Dec 4, 2022
 *      Author: mingy
 */

#ifndef MODULES_CANMESSAGELOOKUPMODULE_H_
#define MODULES_CANMESSAGELOOKUPMODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ApplicationTypes.h"

// CAN Message IDs for each piece of data
// Details can be found on the CAN Reference Document
#define NUMBER_CAN_MESSAGE_IDS 10

typedef enum ICommsMessageLookUpIndex {
    THROTTLE_DATA_ID = 0,
    SPEED_DATA_ID,
    MOTOR_RPM_DATA_ID,
    EVENT_DATA_ID,
    ERROR_DATA_ID,
    CURRENT_VOLTAGE_DATA_ID,
    LIGHT_DATA_ID,
    PRESSURE_TEMPERATURE_DATA_ID,
    EFFICIENCY_DATA_ID,
    MOTOR_TEMP_DATA_ID
} ICommsMessageLookUpIndex;

typedef uint16_t MessageCANID_t;

typedef struct {
    ICommsMessageLookUpIndex index;
    uint8_t numberOfBytes;

    // Lambda Function. Instructions to execute when a message of this type is received.
    void (* canMessageCallback)(iCommsMessage_t* msg);

} ICommsMessageInfo;

extern const ICommsMessageInfo CANMessageLookUpTable[NUMBER_CAN_MESSAGE_IDS];

const ICommsMessageInfo* CANMessageLookUpGetInfo(ICommsMessageLookUpIndex id);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_CANMESSAGELOOKUPMODULE_H_ */
