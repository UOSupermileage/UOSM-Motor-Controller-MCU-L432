/*
 * DatastoreModule.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 *
 *  This is the module for storing information.
 */

#include <DatastoreModule.h>

#include "MotorModule.h"
#include "SerialDebugDriver.h"

// ===== Motor Data =====

PRIVATE MotorDatastoreConfigTypeDef motorDatastoreConfig = {
		0x0000, // Initial throttle value
		0x0000  // Initial target velocity
};

PRIVATE MotorStatusTypeDef motorStatus = {
		0x0000
};

// ===== Motor Config Getters and Setters =====
PUBLIC uint32_t datastoreGetThrottlePercentage() {
	return motorDatastoreConfig.throttle;
}

PUBLIC void datastoreSetThrottlePercentage(uint32_t percentage) {
	if (percentage > 1000) {
		motorDatastoreConfig.throttle = 1000;
		motorDatastoreConfig.targetVelocity = MAX_VELOCITY;
	} else {
		motorDatastoreConfig.throttle = percentage;
		motorDatastoreConfig.targetVelocity = (MAX_VELOCITY - MIN_VELOCITY) / 1000 * percentage + MIN_VELOCITY;
	}
}


PRIVATE void datastoreSetTargetVelocity(int32_t velocity) {

	if (velocity < MIN_VELOCITY) {
		motorDatastoreConfig.targetVelocity = MIN_VELOCITY;
		motorDatastoreConfig.throttle = 0;
	} else if (velocity > MAX_VELOCITY) {
		motorDatastoreConfig.targetVelocity = MAX_VELOCITY;
		motorDatastoreConfig.throttle = 1000;
	} else {
		motorDatastoreConfig.targetVelocity = velocity;
		motorDatastoreConfig.throttle = (velocity - MIN_VELOCITY) / (MAX_VELOCITY - MIN_VELOCITY) * 1000;
	}
}

PRIVATE int32_t datastoreGetTargetVelocity() {
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

