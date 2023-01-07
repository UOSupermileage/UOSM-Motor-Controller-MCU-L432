/*
 * InternalCommsTask.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef TASKS_INTERNALCOMMSTASK_H_
#define TASKS_INTERNALCOMMSTASK_H_

// STM headers
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

// Our headers
#include "ApplicationTypes.h"

PUBLIC void InitInternalCommsTask(void);
PRIVATE void InternalCommsTask(void *argument);

/**
 * Send percentage of torque.
 * 1% = 1 in decimal
 * 50% = 50 in decimal
 */
PRIVATE result_t sendThrottlePercentage(uint8_t percentage);

#endif /* TASKS_INTERNALCOMMSTASK_H_ */
