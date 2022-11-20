/*
 * MotorTask.c
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */


#include "ButtonTask.h"

#include "SerialDebugDriver.h"


// Function alias - replace with the driver api
#define DebugPrint(...) SerialPrintln(__VA_ARGS__)

#define STACK_SIZE 128*4
#define BUTTON_TASK_PRIORITY (osPriority_t) osPriorityHigh2
#define TIMER_BUTTON_TASK 50UL

PUBLIC void InitButtonTask(void);
PRIVATE void ButtonTask(void *argument);

osThreadId_t ButtonTaskHandle;
const osThreadAttr_t ButtonTask_attributes = {
	.name = "ButtonTask",
	.stack_size = STACK_SIZE,
	.priority = BUTTON_TASK_PRIORITY,
};

Button button = {RELEASED, false};

PUBLIC void InitButtonTask(void)
{

	ButtonTaskHandle = osThreadNew(ButtonTask, NULL, &ButtonTask_attributes);

}
PRIVATE void ButtonTask(void *argument)
{
	DebugPrint("Initializing ButtonTask");

	for(;;)
	{
		osSignalWait(buttonDetected, osWaitForever);
		button.state = PRESSED;
		button.debounce = true;

		uint8_t c = 5;
		while(c > 0) {
			osDelay(TIMER_BUTTON_TASK);
			if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) != PIN_SET) {
				c--;
			} else {
				c = 5;
			}
		}

		button.state = RELEASED;
		button.debounce = false;

	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_1)
    {
        if (button.debounce == 0) {
//            osSignalSet(ButtonHandle, USER_BUTTON_DETECTED);
        }
    }
}
