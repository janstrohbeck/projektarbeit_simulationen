#ifndef HAVE_IO_H
#define HAVE_IO_H

#include "data_types.h"

// The logger prints the current temperature and pressure values, as well as
// the current heater and valve settings.
#define DISPLAY_LOGGER 1
// If set to 1, the messages sent to the display are printed in the console
#define DISPLAY_CONSOLE 1

// Starts the simulation.
void init_simulator();
// Halts the current thread for a given number of milliseconds
void delay_ms(int ms);
// Reads the current temperature value and stores it in the variable pointed to by TR.
void read_temp(temp_reading_t *TR);
// Reads the current pressure value and stores it in the variable pointed to by PR.
void read_pressure(pressure_reading_t *PR);
// Sets a new setting for the heater.
void write_heater_setting(heater_setting_t HS);
// Sets a new setting for the valve.
void write_pressure_setting(pressure_setting_t PS);
// Prints a temperature value on the console.
void write_temp_reading(temp_reading_t TR);
// Prints a pressure value on the console.
void write_pressure_reading(pressure_reading_t PR);

#endif
