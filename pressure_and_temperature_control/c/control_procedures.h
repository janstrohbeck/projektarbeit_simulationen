#ifndef HAVE_CONTROL_PROCEDURES_H
#define HAVE_CONTROL_PROCEDURES_H

#include "data_types.h"

// Calculates the correct heater setting if the temperature is to be regulated
// to 20 (degrees). Requires the current temperature (TR) and stores the result
// in the variable pointed to by HS.
void convert_temp(temp_reading_t TR, heater_setting_t *HS);

// Calculates the correct valve setting if the pressure is to be regulated to
// 1000 (mbar). Requires the current pressure (PR) and stores the result in the
// variable pointed to by PS.
void convert_pressure(pressure_reading_t PR, pressure_setting_t *PS);

#endif
