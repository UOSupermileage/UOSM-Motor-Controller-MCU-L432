/*
 * DatastoreModule.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 *
 *  This is the module for storing information.
 */

#include "DataAggregationModule.h"
#include "MotorParameters.h"

// ===== Motor Data =====

PRIVATE motor_config_t motorConfig;


PRIVATE motor_status_t motorStatus;

PUBLIC void InitDataAggregator() {
	#ifdef MOTOR_FIXED_THROTTLE
		motorConfig.throttle = MOTOR_FIXED_THROTTLE;
		motorConfig.ignoreThrottle = Set;
	#else
		motorConfig.ignoreThrottle = Clear;
	#endif

	motorConfig.mode = MOTOR_MODE;
	motorConfig.motionMode = MOTOR_CONFIG_MODE_RAMP_MODE_MOTION;
	motorConfig.maxVelocity = MAX_VELOCITY;
	SystemSetReverseVelocity(Set);
	SystemSetMotorInitializing(Set);

}

// ===== Motor Config Getters and Setters =====
PUBLIC percentage_t SystemGetThrottlePercentage() {
	return motorConfig.throttle;
}

PUBLIC void SystemSetThrottlePercentage(percentage_t percentage) {
	
	if (motorConfig.ignoreThrottle) {
		DebugPrint("Ignoring thorttle input...");
		return;
	}
	
	if (percentage > MAX_PERCENTAGE) {
		motorConfig.throttle = MAX_PERCENTAGE;
	} else {
		motorConfig.throttle = percentage;
	}
}

PUBLIC MotorMode SystemGetMotorMode() {
	return motorConfig.mode;
}
PUBLIC void SystemSetMotorMode(MotorMode mode) {
	motorConfig.mode = mode;
}

PUBLIC flag_status_t SystemGetIgnoreThrottle() {
	return motorConfig.ignoreThrottle;
}
PUBLIC void SystemSetIgnoreThrottle(flag_status_t ignore) {
	motorConfig.ignoreThrottle = ignore;
}

PUBLIC uint16_t SystemGetMaxVelocity() {
	return motorConfig.maxVelocity;
}
PUBLIC void SystemSetMaxVelocity(uint16_t velocity) {
	motorConfig.maxVelocity = velocity;
}

PUBLIC flag_status_t SystemGetReverseVelocity() {
	return motorConfig.reverseVelocity;
}
PUBLIC void SystemSetReverseVelocity(flag_status_t reverse) {
	motorConfig.reverseVelocity = reverse;
}

PUBLIC uint8_t SystemGetMotionMode() {
	return motorConfig.motionMode;
}
PUBLIC void SystemSetMotionMode(uint8_t mode) {
	motorConfig.motionMode = mode;
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
	DebugPrint("Getting Throttle Error %d", motorStatus.throttleError);
	return motorStatus.throttleError;
}

PUBLIC void SystemSetThrottleError(flag_status_t status) {
	motorStatus.throttleError = status;
	DebugPrint("Setting Throttle Error %d", motorStatus.throttleError);

}

PUBLIC flag_status_t  SystemGetMotorInitializing() {
	return motorStatus.motorInitializing;
}
PUBLIC void SystemSetMotorInitializing(flag_status_t status) {
	motorStatus.motorInitializing = status;
}
