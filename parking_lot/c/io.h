#ifndef HAVE_IO_H
#define HAVE_IO_H

#include "data_types.h"

// The logger prints the current number of cars in the parking lot, the
// signal state and the number of cars in the queues, whenever any of these
// values change.
#define DISPLAY_LOGGER 1
// If set to 1, this displays additional messages about the state of the
// gate simulators, e.g. which user events they are waiting for.
#define DISPLAY_GATE_STATE 0
// If set to 1, cars print which actions they are going to take
#define DISPLAY_CAR_STATE 0

// Starts the simulation.
void init_simulator();
// A synchronized version of printf.
int sync_printf(const char *format, ...);
// Halts the current thread for a given number of milliseconds
void delay_ms(int ms);
// Reads if a request to enter was made by a car. The result will be stored in
// the variable pointed to by ER.
void read_entry_request(bool *ER);
// Reads if a request to exit was made by a car. The result will be stored in
// the variable pointed to by ER.
void read_exit_request(bool *ER);
// Reads if a car is currently driving through the entry gate. The result will
// be stored in the variable pointed to by ESS.
void read_entry_sensor_state(bool *ESS);
// Reads if a car is currently driving through the exit gate. The result will
// be stored in the variable pointed to by ESS.
void read_exit_sensor_state(bool *ESS);
// Sets the state of the entry gate to the value of E (opens/closes the gate).
void write_entry_gate_state(gate_state_t E);
// Sets the state of the exit gate to the value of E (opens/closes the gate).
void write_exit_gate_state(gate_state_t E);
// Sets the state of the signal to the value of S (free/full).
void write_signal_state(signal_state_t S);

#endif
