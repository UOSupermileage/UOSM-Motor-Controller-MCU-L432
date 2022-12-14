/*
 * DatastoreModule.h
 *
 *  Created on: Nov 21, 2022
 *      Author: Jeremy Cote
 */

#ifndef MODULES_DATASTOREMODULE_H_
#define MODULES_DATASTOREMODULE_H_

#include <ApplicationTypes.h>

typedef enum MotorStatusCode {
	Clear = 0,
	Set = 1
} MotorStatusCode;

typedef struct {
		int32_t throttle; // Percentage value. Ranges from 0 to 1000. 1 = 0.1%. Thus 0% to 100%.
		int32_t targetVelocity;
} MotorDatastoreConfigTypeDef;

typedef union {
	uint32_t all;
	struct {
		uint32_t iCommsError:1;
		uint32_t spiError:1;
		uint32_t safetyError:1;
		uint32_t reserved;
	};
} MotorStatusTypeDef;

// ===== Motor Config Getters and Setters =====

PUBLIC uint32_t datastoreGetThrottlePercentage();
PUBLIC void datastoreSetThrottlePercentage(uint32_t throttle);

PUBLIC int32_t datastoreGetTargetVelocity();
PRIVATE void datastoreSetTargetVelocity(int32_t velocity);

// ===== Motor Status Getters and Setters =====

PUBLIC uint32_t datastoreGetiCommsError();

PUBLIC void datastoreSetiCommsError(MotorStatusCode status);

PUBLIC uint32_t datastoreGetSPIError();

PUBLIC void datastoreSetSPIError(MotorStatusCode status);

PUBLIC uint32_t datastoreGetSafetyError();

PUBLIC void datastoreSetSafetyError(MotorStatusCode status);


#endif /* MODULES_DATASTOREMODULE_H_ */
