/*
 * MotorTask.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef TASKS_BUTTONTASK_H_
#define TASKS_BUTTONTASK_H_

// STM headers
#include "stm32f4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"
#include "SPIMotorDriver.h"

//uint32_t buttonDetected = 0x0100;

PRIVATE typedef enum ButtonState {
	RELEASED = 0,
	DETECTED = 1,
	PRESSED = 2
} ButtonState;

PRIVATE typedef struct Button {
	ButtonState state;
	bool debounce;
} Button;

PUBLIC void InitButtonTask(void);
PRIVATE void ButtonTask(void *argument);

#endif /* TASKS_BUTTONTASK_H_ */
