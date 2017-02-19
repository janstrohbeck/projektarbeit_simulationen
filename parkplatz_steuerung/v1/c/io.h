#ifndef HAVE_IO_H
#define HAVE_IO_H

#include "data_types.h"

void init_simulator();
int sync_printf(const char *format, ...);
void delay_ms(int ms);
void read_einfahrt_anfrage(bool *EA);
void read_ausfahrt_anfrage(bool *AA);
void read_einfahrt_durchfahrt(bool *ED);
void read_ausfahrt_durchfahrt(bool *AD);
void write_einfahrt(gate_state_t E);
void write_ausfahrt(gate_state_t A);
void write_signal(signal_state_t S);

#endif
