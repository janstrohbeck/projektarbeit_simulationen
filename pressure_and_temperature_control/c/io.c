#include "io.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Mutex that protects the temp_state
static pthread_mutex_t temp_mutex;
// Contains the current state of the temp simulator
static struct {
    temp_reading_t temp;
    heater_setting_t setting;
} temp_state;

// Getters and setters for the temp_state's attributes

static temp_reading_t get_temp() {
    pthread_mutex_lock(&temp_mutex);
    temp_reading_t TR = temp_state.temp;
    pthread_mutex_unlock(&temp_mutex);
    return TR;
}

static void set_temp(temp_reading_t TR) {
    pthread_mutex_lock(&temp_mutex);
    temp_state.temp = TR;
    pthread_mutex_unlock(&temp_mutex);
}

static heater_setting_t get_heater_setting() {
    pthread_mutex_lock(&temp_mutex);
    heater_setting_t HS = temp_state.setting;
    pthread_mutex_unlock(&temp_mutex);
    return HS;
}

static void set_heater_setting(heater_setting_t HS) {
    pthread_mutex_lock(&temp_mutex);
    temp_state.setting = HS;
    pthread_mutex_unlock(&temp_mutex);
}

// Mutex that protects the pressure_state
static pthread_mutex_t pressure_mutex;
// Contains the current state of the pressure simulator
static struct {
    pressure_reading_t pressure;
    pressure_setting_t setting;
} pressure_state;

// Getters and setters for the pressure_state's attributes

static pressure_reading_t get_pressure() {
    pthread_mutex_lock(&pressure_mutex);
    pressure_reading_t PR = pressure_state.pressure;
    pthread_mutex_unlock(&pressure_mutex);
    return PR;
}

static void set_pressure(pressure_reading_t PR) {
    pthread_mutex_lock(&pressure_mutex);
    pressure_state.pressure = PR;
    pthread_mutex_unlock(&pressure_mutex);
}

static pressure_setting_t get_pressure_setting() {
    pthread_mutex_lock(&pressure_mutex);
    pressure_setting_t PS = pressure_state.setting;
    pthread_mutex_unlock(&pressure_mutex);
    return PS;
}

static void set_pressure_setting(pressure_setting_t PS) {
    pthread_mutex_lock(&pressure_mutex);
    pressure_state.setting = PS;
    pthread_mutex_unlock(&pressure_mutex);
}

// Mutex that protects the random seed
static pthread_mutex_t random_mutex;
// The current state of the random generator
static unsigned int rand_seed;

// Generates a new random number between 0 and 99
static int gen_random() {
    pthread_mutex_lock(&random_mutex);
    int res = rand_r(&rand_seed);
    pthread_mutex_unlock(&random_mutex);
    static const int min = 0;
    static const int max = 99;
    return min + res % (max-min+1);
}

static void *temp_simulator(void *args) {
    while (1) {
        temp_reading_t new_temp = get_temp();
        heater_setting_t setting = get_heater_setting();

        // apply random errors (6% probability)
        if (gen_random() < 6) {
            new_temp += gen_random() < 50 ? 4 : -4;
        }
        
        // heat up if heater is on, cool down if heater is off
        new_temp += setting == ON ? 1 : -2;

        // check bounds
        if (new_temp > MAX_TEMP_READING) {
            new_temp = MAX_TEMP_READING;
        } else  if (new_temp < MIN_TEMP_READING) {
            new_temp = MIN_TEMP_READING;
        }

        set_temp(new_temp);
        delay_ms(250);
    }
    return NULL;
}

static void *pressure_simulator(void *args) {
    while (1) {
        pressure_reading_t new_pressure = get_pressure();
        pressure_setting_t setting = get_pressure_setting();

        // apply random errors (10% probability)
        if (gen_random() < 10) {
            new_pressure += gen_random() < 50 ? 30 : -30;
        }

        // increment by the value of the setting
        new_pressure += setting;

        // check bounds
        if (new_pressure > MAX_PRESSURE_READING) {
            new_pressure = MAX_PRESSURE_READING;
        } else if (new_pressure < MIN_PRESSURE_READING) {
            new_pressure = MIN_PRESSURE_READING;
        }
        set_pressure(new_pressure);

        delay_ms(250);
    }
    return NULL;
}

#if DISPLAY_LOGGER
static void *logger(void *args) {
    // periodically print the current state
    while (1) {
        printf("Temp: %d (Heater: %s) | Pressure: %d (Setting: %d)\n",
                get_temp(), get_heater_setting() == ON ? "ON" : "OFF",
                get_pressure(), get_pressure_setting());
        delay_ms(250);
    }
    return NULL;
}
#endif

void init_simulator() {
    srand(time(NULL));

    pthread_mutex_init(&temp_mutex, NULL);
    temp_state.temp = 0;
    temp_state.setting = OFF;

    pthread_t temp_simulator_thread;
    if (pthread_create(&temp_simulator_thread, NULL, temp_simulator, NULL) != 0) exit(EXIT_FAILURE);

    pthread_mutex_init(&pressure_mutex, NULL);
    pressure_state.pressure = 800;
    pressure_state.setting = 0;

    pthread_t pressure_simulator_thread;
    if (pthread_create(&pressure_simulator_thread, NULL, pressure_simulator, NULL) != 0) exit(EXIT_FAILURE);

#if DISPLAY_LOGGER
    pthread_t logger_thread;
    if (pthread_create(&logger_thread, NULL, logger, NULL) != 0) exit(EXIT_FAILURE);
#endif
}

void delay_ms(int ms) {
    nanosleep((const struct timespec[]){{0, ms*1000000L}}, NULL);
}

// The following procedures just read from the state objects

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
    set_heater_setting(HS);
    //printf("Wrinting heater setting: %s\n", HS == ON ? "ON" : "OFF");
}

void write_pressure_setting(pressure_setting_t PS) {
    set_pressure_setting(PS);
    //printf("Wrinting pressure setting: %d\n", PS);
}

void write_temp_reading(temp_reading_t TR) {
#if DISPLAY_CONSOLE
    printf("Console: Read Temp: %d\n", TR);
#endif
}

void write_pressure_reading(pressure_reading_t PR) {
#if DISPLAY_CONSOLE
    printf("Console: Read Pressure: %d\n", PR);
#endif
}
