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

result_t IComms_Init();

result_t IComms_ReceiveNextMessage(iCommsMessage_t* rxMsg);

void IComms_PeriodicReceive();

result_t IComms_Transmit(iCommsMessage_t* txMsg);

iCommsMessage_t IComms_CreateMessage(uint16_t standardMessageID, uint8_t dataLength, uint8_t data[8]);

iCommsMessage_t IComms_CreatePercentageMessage(uint16_t standardMessageID, percentage_t percentage);

iCommsMessage_t IComms_CreateUint32BitMessage(uint16_t standardMessageID, uint32_t value);

iCommsMessage_t IComms_CreateInt32BitMessage(uint16_t standardMessageID, int32_t value);

/**
 * 4 bits -> 16 devices
 * 4 bits -> 16 fault categories
 * 24 bits -> data mask
 * 32 bits -> pointer
 * @param standardMessageID
 * @param code
 * @return
 */
iCommsMessage_t IComms_CreateErrorMessage(uint8_t device, uint8_t code, uint32_t flags, uint32_t data);

iCommsMessage_t IComms_CreateEventMessage(uint8_t code, uint8_t status);

iCommsMessage_t IComms_CreatePairUInt16BitMessage(uint16_t standardMessageID, uint16_t a, uint16_t b);

iCommsMessage_t IComms_CreateLightsMessage(lights_status_t lights);

iCommsMessage_t IComms_CreatePairInt32Message(uint16_t standardMessageID, int32_t a, int32_t b);
result_t IComms_ReadPairInt32Message(iCommsMessage_t *msg, int32_t* a, int32_t* b);

iCommsMessage_t IComms_CreatePressureTemperatureMessage(pressure_t a, temperature_t b);
result_t IComms_ReadPressureTemperatureMessage(iCommsMessage_t* msg, pressure_t* a, temperature_t* b);

/**
 * Create an efficiency message that packs 4 laps into a CAN message
 * @param standardMessageID
 * @param efficiencies
 * @return
 */
iCommsMessage_t IComms_CreateEfficiencyMessage(lap_efficiencies_t* efficiencies);

result_t IComms_ReadEfficiencyMessage(iCommsMessage_t *msg, lap_efficiencies_t* result);

typedef union {
    uint32_t all;
    struct {
        uint16_t a:16;
        uint16_t b:16;
    };
} uint16_pair_t;

uint16_pair_t readMsgPairUInt16Bit(iCommsMessage_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_INTERNALCOMMSMODULE_H_ */
