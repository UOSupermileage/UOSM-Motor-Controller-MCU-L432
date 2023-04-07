/*
 * SafetyModule.h
 *
 *  Created on: Jan 24, 2023
 *      Author: jeremycote
 */

#ifndef MODULES_SAFETYMODULE_H_
#define MODULES_SAFETYMODULE_H_

#include "ApplicationTypes.h"

#define SAFETY_THROTTLE_PERCENTAGE (percentage_t) 300

PUBLIC result_t Safety_SetThrottlePercentage(percentage_t percentage);

#endif /* MODULES_SAFETYMODULE_H_ */
