/*
 * SafetyTask.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef TASKS_SAFETYTASK_H_
#define TASKS_SAFETYTASK_H_

// Our headers
#include "ApplicationTypes.h"

PUBLIC void InitSafetyTask(void);
PRIVATE void SafetyTask(void *argument);

#endif /* TASKS_SAFETYTASK_H_ */
