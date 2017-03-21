#ifndef HAVE_DATA_TYPES_H
#define HAVE_DATA_TYPES_H

#ifndef __cplusplus

typedef enum {
    false,
    true
} bool;

#endif

#define NUM_PARKING_SPOTS 10

typedef enum {
    GATE_CLOSED,
    GATE_OPEN
} gate_state_t;

typedef enum {
    SIGNAL_FREE,
    SIGNAL_FULL
} signal_state_t;

typedef enum {
    SENSOR_FREE,
    SENSOR_BLOCKED
} sensor_state_t;

#endif
