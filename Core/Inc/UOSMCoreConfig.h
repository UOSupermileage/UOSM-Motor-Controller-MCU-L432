// STM headers

#ifndef UOSM_CORE_CONFIG
#define UOSM_CORE_CONFIG

#include "stm32l4xx_hal.h"

#define DEBUG_UART              huart2

#define EXT_SPI_CAN             hspi1
#define SPI_CAN                 &hspi1
#define SPI_TIMEOUT             10
#define MCP2515_CS_HIGH()       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)
#define MCP2515_CS_LOW()        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)

#endif
