/*
 * InternalCommsModule.h
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 */

#ifndef MODULES_INTERNALCOMMSMODULE_H_
#define MODULES_INTERNALCOMMSMODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ApplicationTypes.h"

PUBLIC result_t IComms_Init();

PUBLIC result_t IComms_ReceiveNextMessage(iCommsMessage_t* rxMsg);

PUBLIC void IComms_PeriodicReceive();

PUBLIC result_t IComms_Transmit(iCommsMessage_t* txMsg);

PUBLIC iCommsMessage_t IComms_CreateMessage(uint16_t standardMessageID, uint8_t dataLength, uint8_t data[8]);

PUBLIC iCommsMessage_t IComms_CreatePercentageMessage(uint16_t standardMessageID, percentage_t percentage);

PUBLIC iCommsMessage_t IComms_CreateUint32BitMessage(uint16_t standardMessageID, uint32_t value);

PUBLIC iCommsMessage_t IComms_CreateInt32BitMessage(uint16_t standardMessageID, int32_t value);

PUBLIC iCommsMessage_t IComms_CreateErrorMessage(uint16_t standardMessageID, ErrorCode code, flag_status_t);

PUBLIC iCommsMessage_t IComms_CreateEventMessage(uint16_t standardMessageID, uint8_t code, uint8_t status);

PUBLIC iCommsMessage_t IComms_CreatePairUInt16BitMessage(uint16_t standardMessageID, uint16_t a, uint16_t b);

typedef union {
    uint32_t all;
    struct {
        uint16_t a:16;
        uint16_t b:16;
        uint32_t reserved;
    };
} uint16_pair_t;

PUBLIC uint16_pair_t readMsgPairUInt16Bit(iCommsMessage_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_INTERNALCOMMSMODULE_H_ */
