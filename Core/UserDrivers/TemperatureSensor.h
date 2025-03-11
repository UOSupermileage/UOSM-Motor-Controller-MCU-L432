//
// Created by David on 2025-01-28.
//

#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include "ApplicationTypes.h"

typedef enum {
    TEMP_TOP,
    TEMP_BOTTOM,
} TemperatureSensor;

result_t TemperatureInit();

result_t TemperatureGetTemp(TemperatureSensor sensor, temperature_t *temp);

#endif //TEMPERATURESENSOR_H
