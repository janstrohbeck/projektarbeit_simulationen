#include "control_procedures.h"
#include "io.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *temp_controller(void *args) {
    while (1) {
        temp_reading_t reading;
        read_temp(&reading);
        heater_setting_t setting;
        convert_temp(reading, &setting);
        write_heater_setting(setting);
        write_temp_reading(reading);
        delay_ms(100);
    }
    return NULL;
}

void *pressure_controller(void *args) {
    while (1) {
        pressure_reading_t reading;
        read_pressure(&reading);
        pressure_setting_t setting;
        convert_pressure(reading, &setting);
        write_pressure_setting(setting);
        write_pressure_reading(reading);
        delay_ms(100);
    }
    return NULL;
}

int main() {
    pthread_t temp_controller_thread;
    pthread_t pressure_controller_thread;

    // Try to create threads
    if (pthread_create(&temp_controller_thread, NULL, temp_controller, NULL) != 0) exit(EXIT_FAILURE);
    if (pthread_create(&pressure_controller_thread, NULL, pressure_controller, NULL) != 0) exit(EXIT_FAILURE);

    init_simulator();

    // Exit while allowing the other threads to continue
    pthread_exit(NULL);
}
