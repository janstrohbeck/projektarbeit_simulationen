#ifndef HAVE_DATA_TYPES_H
#define HAVE_DATA_TYPES_H

#ifndef __cplusplus

//Allows usage of boolean values.
typedef enum {
    false,
    true
} bool;

#endif

// The number of parking spots that the parking lot has
#define NUM_PARKING_SPOTS 10

// Used to specify the state of a gate (open or closed).
typedef enum {
    GATE_CLOSED,
    GATE_OPEN
} gate_state_t;

// Used to specify the state of a gate sensor (free or blocked [by a car]).
typedef enum {
    SIGNAL_FREE,
    SIGNAL_FULL
} signal_state_t;

// Used to specify the state of the signal (free or full).
typedef enum {
    SENSOR_FREE,
    SENSOR_BLOCKED
} sensor_state_t;

#endif
