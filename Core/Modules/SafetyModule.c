/*
 * SafetyModule.c
 *
 *  Created on: Jan 24, 2023
 *      Author: jeremycote
 */


#include "SafetyModule.h"

#include "DataAggregationModule.h"

#include "SerialDebugDriver.h"
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

static const char SFT_TAG[] = "#SFT:";

PUBLIC result_t Safety_SetThrottlePercentage(percentage_t percentage) {
	if (SystemGetThrottlePercentage() == 0 && percentage >= SAFETY_THROTTLE_PERCENTAGE) {
		DebugPrint("%s Desired throttle value is too high a jump [%d]. Ignoring...", SFT_TAG, percentage);

		SystemSetThrottleError(Set);
		return RESULT_FAIL;
	}

	SystemSetThrottlePercentage(percentage);

	SystemSetThrottleError(Clear);
	return RESULT_OK;
}
