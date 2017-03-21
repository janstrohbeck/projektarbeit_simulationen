#ifndef HAVE_DATA_TYPES_H
#define HAVE_DATA_TYPES_H

#define MIN_PRESSURE_READING 0
#define MAX_PRESSURE_READING 1200
#define MIN_TEMP_READING 0
#define MAX_TEMP_READING 500
#define MIN_PRESSURE_SETTING -4
#define MAX_PRESSURE_SETTING 4

typedef int pressure_reading_t;
typedef int pressure_setting_t;
typedef int temp_reading_t;
typedef enum {
    OFF,
    ON
} heater_setting_t;

#endif
