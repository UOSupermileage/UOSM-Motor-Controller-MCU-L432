#include "ApplicationTypes.h"

#include "CANMessageLookUpModule.h"
#include "SafetyModule.h"
#include "CANDriver.h"
#include "DataAggregationModule.h"

void ThrottleDataCallback(iCommsMessage_t* msg)
{
	DebugPrint("ThrottleDataCallback! %d", msg->standardMessageID);
	// DebugPrint("Throttle Raw: [%x][%x] length: [%d]", msg.data[1], msg.data[0], msg.dataLength){}
	uint32_t throttle = readMsg(msg);
	DebugPrint("CAN Throttle percentage received: %d", throttle);
	Safety_SetThrottlePercentage(throttle);
}

void EventDataCallback(iCommsMessage_t *msg) {
    DebugPrint("EventDataCallback! %d", msg->standardMessageID);

    if (msg->dataLength == CANMessageLookUpTable[EVENT_DATA_ID].numberOfBytes) {
        EventCode code = msg->data[1];
        flag_status_t status = msg->data[0];

        DebugPrint("EventDataCallback, received code %d with status %d", code, status);
        switch (code) {
        case DRIVER_ENABLED:
            SystemSetDriverEnabled(status);
            break;
        default:
            break;
        }
    } else {
        DebugPrint("msg.dataLength does not match lookup table. %d != %d", msg->dataLength, CANMessageLookUpTable[ERROR_DATA_ID].numberOfBytes);
    }
}

void ErrorDataCallback(iCommsMessage_t *msg){}
void SpeedDataCallback(iCommsMessage_t *msg){}
void MotorRPMDataCallback(iCommsMessage_t *msg){}
void CurrentVoltageDataCallback(iCommsMessage_t *msg){}
void PressureTemperatureDataCallback(iCommsMessage_t *msg){}
void LightsDataCallback(iCommsMessage_t *msg){}
void EfficiencyDataCallback(iCommsMessage_t *msg){}
void MotorTemperatureDataCallback(iCommsMessage_t *msg){}