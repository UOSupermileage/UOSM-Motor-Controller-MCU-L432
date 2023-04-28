#include "ApplicationTypes.h"

#include "CANMessageLookUpModule.h"
#include "SafetyModule.h"
#include "CANDriver.h"

void ThrottleDataCallback(iCommsMessage_t* msg)
{
	DebugPrint("ThrottleDataCallback! %d", msg->standardMessageID);
	// DebugPrint("Throttle Raw: [%x][%x] length: [%d]", msg.data[1], msg.data[0], msg.dataLength);
	uint32_t throttle = readMsg(msg);
	DebugPrint("CAN Throttle percentage received: %d", throttle);
	Safety_SetThrottlePercentage(throttle);
}
