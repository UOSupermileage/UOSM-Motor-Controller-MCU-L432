/*
 * DatastoreModule.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 *
 *  This is the module for storing information.
 */

#include "DataAggregationModule.h"
#include "SerialDebugDriver.h"
#include "SPIMotorDriver.h"

#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

// ===== Motor Data =====

PRIVATE motor_config_t motorConfig;


PRIVATE motor_status_t motorStatus;

// ===== Motor Config Getters and Setters =====
PUBLIC percentage_t SystemGetThrottlePercentage() {
	return motorConfig.throttle;
}

PUBLIC void SystemSetThrottlePercentage(percentage_t percentage) {
	if (percentage > MAX_PERCENTAGE) {
		motorConfig.throttle = MAX_PERCENTAGE;
		motorConfig.targetVelocity = MAX_VELOCITY;
	} else {
		motorConfig.throttle = percentage;
		motorConfig.targetVelocity = (MAX_VELOCITY - MIN_VELOCITY) / MAX_PERCENTAGE * percentage + MIN_VELOCITY;
	}

	// DebugPrint("Value: [%d]", motorConfig.targetVelocity);
}


PRIVATE void SystemSetTargetVelocity(velocity_t velocity) {

	if (velocity < MIN_VELOCITY) {
		motorConfig.targetVelocity = MIN_VELOCITY;
		motorConfig.throttle = MIN_PERCENTAGE;
	} else if (velocity > MAX_VELOCITY) {
		motorConfig.targetVelocity = MAX_VELOCITY;
		motorConfig.throttle = MAX_PERCENTAGE;
	} else {
		motorConfig.targetVelocity = velocity;
		motorConfig.throttle = (velocity - MIN_VELOCITY) / (MAX_VELOCITY - MIN_VELOCITY) * MAX_PERCENTAGE;
	}
}

PRIVATE velocity_t SystemGetTargetVelocity() {
	return motorConfig.targetVelocity;
}

// ===== Motor Status Getters and Setters =====
PUBLIC const uint32_t SystemGetStatusCode() {
	return motorStatus.all;
}

PUBLIC flag_status_t SystemGetiCommsError() {
	return motorStatus.iCommsError;
}

PUBLIC void SystemSetiCommsError(flag_status_t status) {
	motorStatus.iCommsError = status;
}

PUBLIC flag_status_t SystemGetSPIError() {
	return motorStatus.spiError;
}

PUBLIC void SystemSetSPIError(flag_status_t status) {
	motorStatus.spiError = status;
}

PUBLIC flag_status_t SystemGetSafetyError() {
	return motorStatus.safetyError;
}

PUBLIC void SystemSetSafetyError(flag_status_t status) {
	motorStatus.safetyError = status;
}

PUBLIC flag_status_t SystemGetThrottleError() {
	return motorStatus.throttleError;
}

PUBLIC void SystemSetThrottleError(flag_status_t status) {
	motorStatus.safetyError = status;
}
