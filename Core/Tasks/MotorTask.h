/*
 * MotorTask.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef TASKS_MOTORTASK_H_
#define TASKS_MOTORTASK_H_

// STM headers
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"
#include "SPIMotorDriver.h"

void InitMotorTask(void);
void MotorTask(void *argument);

#endif /* TASKS_MOTORTASK_H_ */
