//
// Created by David on 2025-01-28.
//

#include "TemperatureSensor.h"

#define TOP_SENSOR_ADDRESS 0x90
#define BOTTOM_SENSOR_ADDRESS 0x92

extern I2C_HandleTypeDef hi2c1;

static result_t write_register(uint8_t address, uint8_t reg, uint16_t data) {
    uint8_t buffer[3] = {reg, data >> 8, data};
    return HAL_I2C_Master_Transmit(&hi2c1, address, buffer, 3, 1000) == HAL_OK;
}

static result_t read_register(uint8_t address, uint8_t reg, int16_t *data) {
    uint8_t buffer[2] = {0, 0};

    if (HAL_I2C_Master_Transmit(&hi2c1, address, &reg, 2, 1000) != HAL_OK) {
        return RESULT_FAIL;
    }

    if (HAL_I2C_Master_Receive(&hi2c1, address, buffer, 2, 1000) != HAL_OK) {
        return RESULT_FAIL;
    }

    *data = buffer[0] << 8 | buffer[1];

    return HAL_OK;
}


result_t TemperatureInit() {
    // Start continuous conversion
    return write_register(TOP_SENSOR_ADDRESS, 0x01, 0x0220);
}

result_t TemperatureGetTemp(TemperatureSensor sensor, temperature_t *temp) {
    int16_t codes;
    result_t result = read_register(TOP_SENSOR_ADDRESS, 0x00, &codes);

    // Codes per mC, div by 10 to get centi C
    float t = codes * 7.8125f / 10;

    *temp = t;

    return result;
}