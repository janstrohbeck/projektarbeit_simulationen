#include "io.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define ANZAHL_PARKPLAETZE 10

bool EA = false;
bool AA = false;
bool ED = false;
bool AD = false;
int PP = 0;

void *einfahrt(void *args) {
    bool EA;
    bool E;
    bool ED;

    while (1) {
        read_einfahrt_anfrage(&EA);
        if (EA) {
            if (PP < ANZAHL_PARKPLAETZE) {
                write_einfahrt(OPEN);
                ED = true;
                PP++;
                while (ED) {
                    read_einfahrt_durchfahrt(&ED);
                    delay_ms(100);
                }
                write_einfahrt(CLOSE);
            }
        }
        delay_ms(100);
    }
    return NULL;
}

void *ausfahrt(void *args) {
    bool AA;
    bool A;
    bool AD;

    while (1) {
        read_ausfahrt_anfrage(&AA);
        if (AA) {
            write_ausfahrt(OPEN);
            AD = true;
            PP--;
            while (AD) {
                read_ausfahrt_durchfahrt(&AD);
                delay_ms(100);
            }
            write_ausfahrt(CLOSE);
        }
        delay_ms(100);
    }
    return NULL;
}

void *signal(void *args) {
    while (1) {
        if (PP < ANZAHL_PARKPLAETZE) {
            write_signal(FREE);
        } else {
            write_signal(FULL);
        }
        delay_ms(100);
    }
}

int main() {
    init_simulator();

    pthread_t einfahrt_thread;
    pthread_t ausfahrt_thread;
    pthread_t signal_thread;

    // Try to create threads
    if (pthread_create(&einfahrt_thread, NULL, einfahrt, NULL) != 0) exit(EXIT_FAILURE);
    if (pthread_create(&ausfahrt_thread, NULL, ausfahrt, NULL) != 0) exit(EXIT_FAILURE);
    if (pthread_create(&signal_thread, NULL, signal, NULL) != 0) exit(EXIT_FAILURE);

    // Exit while allowing the other threads to continue
    pthread_exit(NULL);
}

