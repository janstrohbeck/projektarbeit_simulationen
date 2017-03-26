#include "io.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int parking_spots = 0;

void *entry_controller(void *args) {
    bool entry_request;
    bool entry_sensor_state;

    while (1) {
        read_entry_request(&entry_request);
        // If a car wants to enter
        if (entry_request) {
            // Check if parking lot is not already full
            if (parking_spots < NUM_PARKING_SPOTS) {
                parking_spots++;
                // Open gate
                write_entry_gate_state(GATE_OPEN);
                // Wait until the sensor is not blocked any more (car must be through then)
                entry_sensor_state = true;
                while (entry_sensor_state) {
                    read_entry_sensor_state(&entry_sensor_state);
                    delay_ms(100);
                }
                // Close gate
                write_entry_gate_state(GATE_CLOSED);
            }
        }
        delay_ms(100);
    }
    return NULL;
}

void *exit_controller(void *args) {
    bool exit_request;
    bool exit_sensor_state;

    while (1) {
        read_exit_request(&exit_request);
        // If a car wants to leave
        if (exit_request) {
            parking_spots--;
            // Open gate
            write_exit_gate_state(GATE_OPEN);
            // Wait until the sensor is not blocked any more (car must be through then)
            exit_sensor_state = true;
            while (exit_sensor_state) {
                read_exit_sensor_state(&exit_sensor_state);
                delay_ms(100);
            }
            // Close gate
            write_exit_gate_state(GATE_CLOSED);
        }
        delay_ms(100);
    }
    return NULL;
}

void *signal_controller(void *args) {
    while (1) {
        if (parking_spots < NUM_PARKING_SPOTS) {
            write_signal_state(SIGNAL_FREE);
        } else {
            write_signal_state(SIGNAL_FULL);
        }
        delay_ms(100);
    }
    return NULL;
}

int main() {
    init_simulator();

    pthread_t entry_controller_thread;
    pthread_t exit_controller_thread;
    pthread_t signal_controller_thread;

    // Try to create threads
    if (pthread_create(&entry_controller_thread, NULL, entry_controller, NULL) != 0) exit(EXIT_FAILURE);
    if (pthread_create(&exit_controller_thread, NULL, exit_controller, NULL) != 0) exit(EXIT_FAILURE);
    if (pthread_create(&signal_controller_thread, NULL, signal_controller, NULL) != 0) exit(EXIT_FAILURE);

    // Exit while allowing the other threads to continue
    pthread_exit(NULL);
}

