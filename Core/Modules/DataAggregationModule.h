/*
 * DatastoreModule.h
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 */

#ifndef MODULES_DATAAGGREGATIONMODULE_H_
#define MODULES_DATAAGGREGATIONMODULE_H_

#include <ApplicationTypes.h>

typedef enum { Clear, Set } flag_status_t;

typedef struct
{
	velocity_t throttle; // Percentage value. Ranges from 0 to 1000. 1 = 0.1%. Thus 0% to 100%.
	velocity_t targetVelocity;
} motor_config_t;

typedef union {
	uint32_t all;
	struct {
		uint32_t iCommsError:1;
		uint32_t spiError:1;
		uint32_t safetyError:1;
		uint32_t throttleError:1; // High if
		uint32_t reserved;
	};
} motor_status_t;

void InitDataAggregator();

// ===== Motor Config Getters and Setters =====

PUBLIC percentage_t SystemGetThrottlePercentage();
PUBLIC void SystemSetThrottlePercentage(percentage_t throttle);

PUBLIC velocity_t SystemGetTargetVelocity();
PRIVATE void SystemSetTargetVelocity(velocity_t velocity);

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

#endif /* MODULES_DATAAGGREGATIONMODULE_H_ */
