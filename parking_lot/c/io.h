#ifndef HAVE_IO_H
#define HAVE_IO_H

#include "data_types.h"

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
