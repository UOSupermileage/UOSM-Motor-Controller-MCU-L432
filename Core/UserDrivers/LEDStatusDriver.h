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

#define LED_STATUS_START_LENGTH 500
#define LED_STATUS_BLINK_LENGTH 100

#define LED_STATUS_PIN GPIOB
#define LED_STATUS_PIN_N GPIO_PIN_3

// STM headers
#include "stm32l4xx_hal.h"

PUBLIC void STATUS_PeriodicDisplayStatusCode(const uint32_t code);

#endif /* USERDRIVERS_LEDSTATUSDRIVER_H_ */
