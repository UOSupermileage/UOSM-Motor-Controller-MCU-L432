/*
 * LEDStatusDriver.h
 *
 *  Created on: Jan. 24, 2023
 *      Author: jeremycote
 */

#ifndef USERDRIVERS_LEDSTATUSDRIVER_H_
#define USERDRIVERS_LEDSTATUSDRIVER_H_

#include "ApplicationTypes.h"
#include "DataAggregationModule.h"

#define LED_A_STATUS_START_LENGTH 500
#define LED_A_STATUS_BLINK_LENGTH 100

#define LED_A_STATUS_PIN GPIOB
#define LED_A_STATUS_PIN_N GPIO_PIN_3

#define LED_B_STATUS_START_LENGTH 500
#define LED_B_STATUS_BLINK_LENGTH 100

#define LED_B_STATUS_PIN GPIOA
#define LED_B_STATUS_PIN_N GPIO_PIN_10

// STM headers
#include "stm32l4xx_hal.h"

PUBLIC void STATUS_PeriodicDisplayStatusCode();

#endif /* USERDRIVERS_LEDSTATUSDRIVER_H_ */
