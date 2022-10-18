/*
 * SPIDriver.h
 *
 *  Created on: Oct 4, 2022
 *      Author: jeremycote
 */

#ifndef USERDRIVERS_SPIDRIVER_H_
#define USERDRIVERS_SPIDRIVER_H_

// STM headers
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

// Our headers
#include "ApplicationTypes.h"

PUBLIC uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer);

#endif /* USERDRIVERS_SPIDRIVER_H_ */
