#ifndef HAVE_IO_H
#define HAVE_IO_H

#include "data_types.h"

void init_simulator();
void delay_ms(int ms);
void read_temp(temp_reading_t *TR);
void read_pressure(pressure_reading_t *PR);
void write_heater_setting(heater_setting_t HS);
void write_pressure_setting(pressure_setting_t PS);
void write_temp_reading(temp_reading_t TR);
void write_pressure_reading(pressure_reading_t PR);

#endif
