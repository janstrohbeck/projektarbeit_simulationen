#ifndef HAVE_DATA_TYPES_H
#define HAVE_DATA_TYPES_H

typedef enum {
    false,
    true
} bool;

typedef enum {
    CLOSE,
    OPEN
} gate_state_t;

typedef enum {
    FREE,
    FULL
} signal_state_t;

#endif
