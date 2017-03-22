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

void init_simulator();
int sync_printf(const char *format, ...);
void delay_ms(int ms);
void read_entry_request(bool *ER);
void read_exit_request(bool *ER);
void read_entry_sensor_state(bool *ESS);
void read_exit_sensor_state(bool *ESS);
void write_entry_gate_state(gate_state_t E);
void write_exit_gate_state(gate_state_t E);
void write_signal_state(signal_state_t S);

#endif
