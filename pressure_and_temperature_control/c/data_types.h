#ifndef HAVE_DATA_TYPES_H
#define HAVE_DATA_TYPES_H

// minimum and maximum pressure sensor values
#define MIN_PRESSURE_READING 0
#define MAX_PRESSURE_READING 1200
// minimum and maximum temperature sensor values
#define MIN_TEMP_READING 0
#define MAX_TEMP_READING 500
// minimum and maximum values for the pressure setting
#define MIN_PRESSURE_SETTING -4
#define MAX_PRESSURE_SETTING 4

// Can be used to represent pressure values. This definition is mainly because
// of readability, as no type-checks are performed by C (as opposed to Ada).
typedef int pressure_reading_t;

// Can be used to represent a setting for the valve. The allowed range for this
// setting is -4 to +4.
typedef int pressure_setting_t;

// Can be used to represent temperature values. This definition is mainly
// because of readability, as no type-checks are performed by C (as opposed to
// Ada).
typedef int temp_reading_t;

// Can be used to represent a setting of the heater. As it is controlled by a
// simple switch, the heater can only be on or off.
typedef enum {
    OFF,
    ON
} heater_setting_t;

#endif
