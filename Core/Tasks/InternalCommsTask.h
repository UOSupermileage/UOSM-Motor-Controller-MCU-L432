/*
 * InternalCommsTask.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef TASKS_INTERNALCOMMSTASK_H_
#define TASKS_INTERNALCOMMSTASK_H_

// STM headers
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"

PUBLIC void InitInternalCommsTask(void);
PRIVATE void InternalCommsTask(void *argument);

#endif /* TASKS_INTERNALCOMMSTASK_H_ */