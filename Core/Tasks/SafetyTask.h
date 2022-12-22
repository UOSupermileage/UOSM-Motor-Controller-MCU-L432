/*
 * SafetyTask.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef TASKS_SAFETYTASK_H_
#define TASKS_SAFETYTASK_H_

// STM headers
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"

PUBLIC void InitSafetyTask(void);

// TODO: Get LP to make table of safe, dangerous, etc ranges of operation.
PRIVATE void SafetyTask(void *argument);

#endif /* TASKS_SAFETYTASK_H_ */
