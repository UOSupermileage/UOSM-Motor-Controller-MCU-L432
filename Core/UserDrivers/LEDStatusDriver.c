/*
 * LEDStatusDriver.c
 *
 *  Created on: Jan. 24, 2023
 *      Author: jeremycote
 */

#include "LEDStatusDriver.h"

PUBLIC void STATUS_PeriodicDisplayStatusCode(const uint32_t code) {

	// LED A Status Codes. Long flash, followed by n quick flashes where n = code
	HAL_GPIO_WritePin(LED_A_STATUS_PIN, LED_A_STATUS_PIN_N, GPIO_PIN_SET);
	HAL_Delay (LED_A_STATUS_START_LENGTH);   /* Insert delay 100 ms */
	HAL_GPIO_WritePin(LED_A_STATUS_PIN, LED_A_STATUS_PIN_N, GPIO_PIN_RESET);

	for (int i = 0; i < code; i++) {
		HAL_Delay (LED_A_STATUS_BLINK_LENGTH);
		HAL_GPIO_WritePin(LED_A_STATUS_PIN, LED_A_STATUS_PIN_N, GPIO_PIN_SET);

		HAL_Delay (LED_A_STATUS_BLINK_LENGTH);
		HAL_GPIO_WritePin(LED_A_STATUS_PIN, LED_A_STATUS_PIN_N, GPIO_PIN_RESET);
	}


	// LED B Status Codes

}
