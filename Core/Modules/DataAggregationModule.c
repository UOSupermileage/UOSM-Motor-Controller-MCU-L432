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

motor_config_t motorConfig;
motor_status_t motorStatus;
velocity_t velocity;

void InitDataAggregator() {
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
percentage_t SystemGetThrottlePercentage() {
	return motorConfig.throttle;
}

void SystemSetThrottlePercentage(percentage_t percentage) {
	
	if (motorConfig.ignoreThrottle) {
		DebugPrint("Ignoring throttle input...");
		return;
	}
	
	if (percentage > MAX_PERCENTAGE) {
		motorConfig.throttle = MAX_PERCENTAGE;
	} else {
		motorConfig.throttle = percentage;
	}
}

MotorMode SystemGetMotorMode() {
	return motorConfig.mode;
}
void SystemSetMotorMode(MotorMode mode) {
	motorConfig.mode = mode;
}

flag_status_t SystemGetIgnoreThrottle() {
	return motorConfig.ignoreThrottle;
}
void SystemSetIgnoreThrottle(flag_status_t ignore) {
	motorConfig.ignoreThrottle = ignore;
}

uint16_t SystemGetMaxVelocity() {
	return motorConfig.maxVelocity;
}
void SystemSetMaxVelocity(uint16_t velocity) {
	motorConfig.maxVelocity = velocity;
}

flag_status_t SystemGetReverseVelocity() {
	return motorConfig.reverseVelocity;
}
void SystemSetReverseVelocity(flag_status_t reverse) {
	motorConfig.reverseVelocity = reverse;
}

uint8_t SystemGetMotionMode() {
	return motorConfig.motionMode;
}
void SystemSetMotionMode(uint8_t mode) {
	motorConfig.motionMode = mode;
}

// ===== Motor Status Getters and Setters =====
const uint32_t SystemGetStatusCode() {
	return motorStatus.all;
}

flag_status_t SystemGetiCommsError() {
	return motorStatus.iCommsError;
}

void SystemSetiCommsError(flag_status_t status) {
	motorStatus.iCommsError = status;
}

flag_status_t SystemGetSPIError() {
	return motorStatus.spiError;
}

void SystemSetSPIError(flag_status_t status) {
	motorStatus.spiError = status;
}

flag_status_t SystemGetSafetyError() {
	return motorStatus.safetyError;
}

void SystemSetSafetyError(flag_status_t status) {
	motorStatus.safetyError = status;
}

flag_status_t SystemGetThrottleError() {
	DebugPrint("Getting Throttle Error %d", motorStatus.throttleError);
	return motorStatus.throttleError;
}

void SystemSetThrottleError(flag_status_t status) {
	motorStatus.throttleError = status;
	DebugPrint("Setting Throttle Error %d", motorStatus.throttleError);

}

flag_status_t  SystemGetMotorInitializing() {
	return motorStatus.motorInitializing;
}
void SystemSetMotorInitializing(flag_status_t status) {
	motorStatus.motorInitializing = status;
}

velocity_t SystemGetMotorVelocity() {
	return velocity;
}
void SystemSetMotorVelocity(velocity_t v) {
	velocity = v;
}

flag_status_t SystemGetDriverEnabled(){
	return motorStatus.driverEnabled;
}
void SystemSetDriverEnabled(flag_status_t enabled) {
	motorStatus.driverEnabled = enabled;
}
