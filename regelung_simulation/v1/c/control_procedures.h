#ifndef HAVE_CONTROL_PROCEDURES_H
#define HAVE_CONTROL_PROCEDURES_H

#include "data_types.h"

void convert_temp(temp_reading_t TR, heater_setting_t *HS);
void convert_pressure(pressure_reading_t PR, pressure_setting_t *PS);

#endif
