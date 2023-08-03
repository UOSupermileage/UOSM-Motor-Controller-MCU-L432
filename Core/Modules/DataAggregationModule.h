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

PUBLIC percentage_t SystemGetThrottlePercentage();
PUBLIC void SystemSetThrottlePercentage(percentage_t throttle);

PUBLIC MotorMode SystemGetMotorMode();
PUBLIC void SystemSetMotorMode(MotorMode mode);

PUBLIC flag_status_t SystemGetIgnoreThrottle();
PUBLIC void SystemSetIgnoreThrottle(flag_status_t ignore);

PUBLIC uint16_t SystemGetMaxVelocity();
PUBLIC void SystemSetMaxVelocity(uint16_t velocity);

PUBLIC flag_status_t SystemGetReverseVelocity();
PUBLIC void SystemSetReverseVelocity(flag_status_t reverse);

PUBLIC uint8_t SystemGetMotionMode();
PUBLIC void SystemSetMotionMode(uint8_t mode);

PUBLIC flag_status_t SystemGetDriverEnabled();
PUBLIC void SystemSetDriverEnabled(flag_status_t enabled);

// ===== Motor Status Getters and Setters =====

PUBLIC const uint32_t SystemGetStatusCode();

PUBLIC flag_status_t SystemGetiCommsError();

PUBLIC void SystemSetiCommsError(flag_status_t status);

PUBLIC flag_status_t SystemGetSPIError();

PUBLIC void SystemSetSPIError(flag_status_t status);

PUBLIC flag_status_t SystemGetSafetyError();

PUBLIC void SystemSetSafetyError(flag_status_t status);

PUBLIC flag_status_t SystemGetThrottleError();

PUBLIC void SystemSetThrottleError(flag_status_t status);

PUBLIC flag_status_t  SystemGetMotorInitializing();
PUBLIC void SystemSetMotorInitializing(flag_status_t status);

PUBLIC velocity_t SystemGetMotorVelocity();
PUBLIC void SystemSetMotorVelocity(velocity_t velocity);

#endif /* MODULES_DATAAGGREGATIONMODULE_H_ */
