/*
 * DatastoreModule.h
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 */

#ifndef MODULES_DATAAGGREGATIONMODULE_H_
#define MODULES_DATAAGGREGATIONMODULE_H_

#include "ApplicationTypes.h"

typedef struct
{
	percentage_t throttle; // Percentage value. Ranges from 0 to 1000. 1 = 0.1%. Thus 0% to 100%.
	MotorMode mode;
	flag_status_t ignoreThrottle;
	uint16_t maxVelocity;
	flag_status_t reverseVelocity;
	uint8_t motionMode;
} motor_config_t;

typedef union {
	uint32_t all;
	struct {
		uint32_t iCommsError:1;
		uint32_t spiError:1;
		uint32_t safetyError:1;
		uint32_t throttleError:1; // High if throttle was set too high (See SafetyModule)
		uint32_t motorInitializing:1;
		uint32_t driverEnabled:1;
		uint32_t reserved;
	};
} motor_status_t;

void InitDataAggregator();

// ===== Motor Config Getters and Setters =====

percentage_t SystemGetThrottlePercentage();
void SystemSetThrottlePercentage(percentage_t throttle);

MotorMode SystemGetMotorMode();
void SystemSetMotorMode(MotorMode mode);

flag_status_t SystemGetIgnoreThrottle();
void SystemSetIgnoreThrottle(flag_status_t ignore);

uint16_t SystemGetMaxVelocity();
void SystemSetMaxVelocity(uint16_t velocity);

flag_status_t SystemGetReverseVelocity();
void SystemSetReverseVelocity(flag_status_t reverse);

uint8_t SystemGetMotionMode();
void SystemSetMotionMode(uint8_t mode);

flag_status_t SystemGetDriverEnabled();
void SystemSetDriverEnabled(flag_status_t enabled);

// ===== Motor Status Getters and Setters =====

const uint32_t SystemGetStatusCode();

flag_status_t SystemGetiCommsError();

void SystemSetiCommsError(flag_status_t status);

flag_status_t SystemGetSPIError();

void SystemSetSPIError(flag_status_t status);

flag_status_t SystemGetSafetyError();

void SystemSetSafetyError(flag_status_t status);

flag_status_t SystemGetThrottleError();

void SystemSetThrottleError(flag_status_t status);

flag_status_t  SystemGetMotorInitializing();
void SystemSetMotorInitializing(flag_status_t status);

velocity_t SystemGetMotorVelocity();
void SystemSetMotorVelocity(velocity_t velocity);

temperature_t SystemGetTemperature();
void SystemSetTemperature(temperature_t temperature);

#endif /* MODULES_DATAAGGREGATIONMODULE_H_ */
