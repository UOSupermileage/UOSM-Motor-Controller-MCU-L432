/*
 * DatastoreModule.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 *
 *  This is the module for storing information.
 */

#include <DatastoreModule.h>

#include "SerialDebugDriver.h"

// ===== Motor Data =====

PRIVATE MotorDatastoreConfigTypeDef motorDatastoreConfig = {
		0x0000 // Initial Target Velocity
};

PRIVATE MotorStatusTypeDef motorStatus = {
		0x0000
};

// ===== Motor Config Getters and Setters =====
PUBLIC uint32_t datastoreGetTargetTorque()
{
	DebugPrint("datastoreGetTargetTorque not implemented!");
	return 0;
}

PUBLIC void datastoreSetTargetTorque(uint32_t torque)
{
	DebugPrint("datastoreSetTargetTorque not implemented!");
	//DebugPrint("Setting target torque to: [%08x]", torque);
	//motorConfig.targetTorque = torque;
}

PUBLIC void datastoreSetTargetTorquePercentage(uint8_t percentage) {
	DebugPrint("datastoreGetTargetTorquePercentage not implemented!");
	// motorConfig.targetTorque = (MAX_TORQUE - MIN_TORQUE) / 100 * percentage + MIN_TORQUE;
}


PUBLIC void datastoreSetTargetVelocity(int32_t velocity) {
	motorDatastoreConfig.targetVelocity = velocity;
}
PUBLIC int32_t datastoreGetTargetVelocity() {
	return motorDatastoreConfig.targetVelocity;
}

// ===== Motor Status Getters and Setters =====
PUBLIC uint32_t datastoreGetiCommsError() {
	return motorStatus.iCommsError;
}

PUBLIC void datastoreSetiCommsError(MotorStatusCode status) {
	motorStatus.iCommsError = status;
}

PUBLIC uint32_t datastoreGetSPIError() {
	return motorStatus.spiError;
}

PUBLIC void datastoreSetSPIError(MotorStatusCode status) {
	motorStatus.spiError = status;
}

PUBLIC uint32_t datastoreGetSafetyError() {
	return motorStatus.safetyError;
}

PUBLIC void datastoreSetSafetyError(MotorStatusCode status) {
	motorStatus.safetyError = status;
}

