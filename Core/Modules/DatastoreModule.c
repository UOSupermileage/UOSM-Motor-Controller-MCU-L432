/*
 * DatastoreModule.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 *
 *  This is the module for storing information.
 */

#include <DatastoreModule.h>

#include "SPIMotorDriver.h"

// ===== Motor Data =====

PRIVATE MotorConfigTypeDef motorConfig = {
		0x0000 // Initial Target Torque
};

PRIVATE MotorStatusTypeDef motorStatus = {
		0x0000
};

// ===== Motor Config Getters and Setters =====
PUBLIC uint32_t datastoreGetTargetTorque()
{
	return motorConfig.targetTorque;
}

PUBLIC void datastoreSetTargetTorque(uint32_t torque)
{
	motorConfig.targetTorque = torque;
}

PUBLIC void datastoreSetTargetTorquePercentage(uint8_t percentage) {
	motorConfig.targetTorque = (MAX_TORQUE - MIN_TORQUE) / 100 * percentage + MIN_TORQUE;
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

