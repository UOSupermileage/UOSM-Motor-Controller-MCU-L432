/*
 * CANMessageLookUpModule.c
 *
 *  Created on: Dec 4, 2022
 *      Author: mingy
 */

#include "CANMessageLookUpModule.h"
#include "CANDriver.h"

#ifndef MBED
#define WEAK __weak
#else
#define WEAK
#endif

extern WEAK void ThrottleDataCallback(iCommsMessage_t *msg);
extern WEAK void ErrorDataCallback(iCommsMessage_t *msg);
extern WEAK void SpeedDataCallback(iCommsMessage_t *msg);
extern WEAK void EventDataCallback(iCommsMessage_t *msg);
extern WEAK void MotorRPMDataCallback(iCommsMessage_t *msg);
extern WEAK void CurrentVoltageDataCallback(iCommsMessage_t *msg);
extern WEAK void PressureTemperatureDataCallback(iCommsMessage_t *msg);
extern WEAK void LightsDataCallback(iCommsMessage_t *msg);
extern WEAK void EfficiencyDataCallback(iCommsMessage_t *msg);

/*********************************************************************************
 *
 * 		Look up table for CAN ID and meta data about its payload
 *
 **********************************************************************************/
const ICommsMessageInfo CANMessageLookUpTable[NUMBER_CAN_MESSAGE_IDS] = {
        // Message Index			CAN ID		Num of Bytes		Callback
        {THROTTLE_DATA_ID,             0x0001, 2, &ThrottleDataCallback},
        {SPEED_DATA_ID,                0x0002, 4, &SpeedDataCallback},
        {MOTOR_RPM_DATA_ID,            0x0003, 4, &MotorRPMDataCallback},
        {EVENT_DATA_ID,                0x0400, 2, &EventDataCallback},
        {ERROR_DATA_ID,                0x0401, 2, &ErrorDataCallback},
        {CURRENT_VOLTAGE_DATA_ID,      0x0004, 2, &CurrentVoltageDataCallback},
        {LIGHT_DATA_ID,                0x0305, 4, &LightsDataCallback},
        {PRESSURE_TEMPERATURE_DATA_ID, 0x0005, 8, &PressureTemperatureDataCallback},
        {EFFICIENCY_DATA_ID,           0x0007, 4, &EfficiencyDataCallback}
};

PUBLIC const ICommsMessageInfo*
CANMessageLookUpGetInfo(ICommsMessageLookUpIndex id) { return &CANMessageLookUpTable[id]; }