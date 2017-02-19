#include "io.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static pthread_mutex_t temp_mutex;
static struct {
    temp_reading_t temp;
    heater_setting_t setting;
} temp_state;

static pthread_mutex_t pressure_mutex;
static struct {
    pressure_reading_t pressure;
    pressure_setting_t setting;
} pressure_state;

static void *temp_simulator(void *args) {
    while (1) {
        printf("Temp simulator!\n");
        pthread_mutex_lock(&temp_mutex);
        if (temp_state.setting == ON) {
            temp_state.temp += 1;
            if (temp_state.temp > MAX_TEMP_READING) {
                temp_state.temp = MAX_TEMP_READING;
            }
        } else {
            temp_state.temp -= 2;
            if (temp_state.temp < MIN_TEMP_READING) {
                temp_state.temp = MIN_TEMP_READING;
            }
        }
        pthread_mutex_unlock(&temp_mutex);
        delay_ms(250);
    }
}

static void *pressure_simulator(void *args) {
    while (1) {
        printf("Pressure simulator!\n");
        pthread_mutex_lock(&pressure_mutex);
        pressure_state.pressure += pressure_state.setting-4;
        if (pressure_state.pressure > MAX_PRESSURE_READING) {
            pressure_state.pressure = MAX_PRESSURE_READING;
        } else if (pressure_state.pressure < MIN_PRESSURE_READING) {
            pressure_state.pressure = MIN_PRESSURE_READING;
        }
        pthread_mutex_unlock(&pressure_mutex);
        delay_ms(250);
    }
}

void init_simulator() {
    pthread_mutex_init(&temp_mutex, NULL);
    temp_state.temp = 30;
    temp_state.setting = OFF;

    pthread_t temp_simulator_thread;
    if (pthread_create(&temp_simulator_thread, NULL, temp_simulator, NULL) != 0) exit(EXIT_FAILURE);

    pthread_mutex_init(&pressure_mutex, NULL);
    pressure_state.pressure = 30;
    pressure_state.setting = 0;

    pthread_t pressure_simulator_thread;
    if (pthread_create(&pressure_simulator_thread, NULL, pressure_simulator, NULL) != 0) exit(EXIT_FAILURE);
}

void delay_ms(int ms) {
    nanosleep((const struct timespec[]){0, ms*1000000L}, NULL);
}

void read_temp(temp_reading_t *TR) {
    pthread_mutex_lock(&temp_mutex);
    *TR = temp_state.temp;
    pthread_mutex_unlock(&temp_mutex);
}

void read_pressure(pressure_reading_t *PR) {
    pthread_mutex_lock(&pressure_mutex);
    *PR = pressure_state.pressure;
    pthread_mutex_unlock(&pressure_mutex);
}

void write_heater_setting(heater_setting_t HS) {
    pthread_mutex_lock(&temp_mutex);
    temp_state.setting = HS;
    pthread_mutex_unlock(&temp_mutex);
    printf("Wrinting heater setting: %s\n", HS == ON ? "ON" : "OFF");
}

void write_pressure_setting(pressure_setting_t PS) {
    pthread_mutex_lock(&pressure_mutex);
    pressure_state.setting = PS;
    pthread_mutex_unlock(&pressure_mutex);
    printf("Wrinting pressure setting: %d\n", PS);
}

void write_temp_reading(temp_reading_t TR) {
    printf("Console: Read Temp: %d\n", TR);
}

void write_pressure_reading(pressure_reading_t PR) {
    printf("Console: Read Pressure: %d\n", PR);
}
