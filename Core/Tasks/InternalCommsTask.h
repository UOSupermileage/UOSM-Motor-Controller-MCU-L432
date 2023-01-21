/*
 * ICommsTask.h
 *
 *  Created on: Sep 10, 2022
 *      Author: mingy
 */

#ifndef TASKS_INTERNALCOMMSTASK_H_
#define TASKS_INTERNALCOMMSTASK_H_

// STM headers
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"


PUBLIC void InitInternalCommsTask(void);
PRIVATE void InternalCommsTask(void *argument);


#endif /* TASKS_INTERNALCOMMSTASK_H_ */
