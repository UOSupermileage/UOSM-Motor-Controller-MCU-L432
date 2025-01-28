/*
 * CANMessageLookUpModule.c
 *
 *  Created on: Dec 4, 2022
 *      Author: mingy
 */

#include "CANMessageLookUpModule.h"

extern void ThrottleDataCallback(iCommsMessage_t *msg);
extern void ErrorDataCallback(iCommsMessage_t *msg);
extern void SpeedDataCallback(iCommsMessage_t *msg);
extern void EventDataCallback(iCommsMessage_t *msg);
extern void MotorRPMDataCallback(iCommsMessage_t *msg);
extern void CurrentVoltageDataCallback(iCommsMessage_t *msg);
extern void PressureTemperatureDataCallback(iCommsMessage_t *msg);
extern void LightsDataCallback(iCommsMessage_t *msg);
extern void EfficiencyDataCallback(iCommsMessage_t *msg);
extern void MotorTemperatureDataCallback(iCommsMessage_t *msg);

/*********************************************************************************
 *
 * 		Look up table for CAN ID and meta data about its payload
 *
 **********************************************************************************/
const ICommsMessageInfo CANMessageLookUpTable[NUMBER_CAN_MESSAGE_IDS] = {
        // Message Index,              Num of Bytes, Callback
        {THROTTLE_DATA_ID,             2, &ThrottleDataCallback},
        {SPEED_DATA_ID,                4, &SpeedDataCallback},
        {MOTOR_RPM_DATA_ID,            4, &MotorRPMDataCallback},
        {EVENT_DATA_ID,                2, &EventDataCallback},
        {ERROR_DATA_ID,                2, &ErrorDataCallback},
        {CURRENT_VOLTAGE_DATA_ID,      4, &CurrentVoltageDataCallback},
        {LIGHT_DATA_ID,                4, &LightsDataCallback},
        {PRESSURE_TEMPERATURE_DATA_ID, 8, &PressureTemperatureDataCallback},
        {EFFICIENCY_DATA_ID,           4, &EfficiencyDataCallback},
        {MOTOR_TEMPERATURE_DATA_ID,    4, &MotorTemperatureDataCallback}
};

const ICommsMessageInfo*
CANMessageLookUpGetInfo(ICommsMessageLookUpIndex id) { return &CANMessageLookUpTable[id]; }