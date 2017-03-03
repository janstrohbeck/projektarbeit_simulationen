#include "io.h"
#include "call_queue.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdarg.h>

#define NUM_CARS 20

static pthread_mutex_t mutex;
static pthread_cond_t cond_E;
static pthread_cond_t cond_A;
static pthread_cond_t cond_no_E;
static pthread_cond_t cond_no_A;

static struct {
    unsigned int rand_seed;
    gate_state_t E;
    gate_state_t A;
    signal_state_t S;
    bool EA;
    bool AA;
    bool ED;
    bool AD;
    int car_cnt;
} simulator_state;

static pthread_mutex_t printf_mutex;
int sync_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    vprintf(format, args);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
}

static void *simulator(void *args) {
    while (1) {
        pthread_mutex_lock(&mutex);
        int car_cnt = simulator_state.car_cnt;
        signal_state_t signal_state = simulator_state.S;
        pthread_mutex_unlock(&mutex);
        sync_printf("Cars inside: %d | Signal: %s\n", car_cnt, signal_state == FREE ? "FREE" : "FULL");
        delay_ms(100);
    }
    return NULL;
}

call_queue_t einfahrt_enter_queue;
sem_t einfahrt_sem_entered;

static void *einfahrt_simulator(void *args) {
    while (1) {
        sem_t *sem_complete = call_queue_receive(&einfahrt_enter_queue);
        sync_printf("E: A car wants to enter!\n");
        
        pthread_mutex_lock(&mutex);
        simulator_state.EA = true;
        pthread_mutex_unlock(&mutex);

        sync_printf("E: Waiting for gate to open...\n");
        pthread_mutex_lock(&mutex);
        while (!simulator_state.E) pthread_cond_wait(&cond_E, &mutex);
        pthread_mutex_unlock(&mutex);

        sync_printf("E: Gate open!\n");
        pthread_mutex_lock(&mutex);
        simulator_state.EA = false;
        pthread_mutex_unlock(&mutex);

        // Notify car that gate has opened
        sem_post(sem_complete);

        sync_printf("E: Waiting for car to drive through...\n");
        sem_wait(&einfahrt_sem_entered);

        sync_printf("E: Car is through!\n");
        pthread_mutex_lock(&mutex);
        simulator_state.ED = false;
        simulator_state.car_cnt++;
        pthread_mutex_unlock(&mutex);

        sync_printf("E: Waiting for gate to close...\n");
        pthread_mutex_lock(&mutex);
        while (simulator_state.E) pthread_cond_wait(&cond_no_E, &mutex);
        pthread_mutex_unlock(&mutex);

        sync_printf("E: Gate closed!\n");
    }
}

static call_queue_t ausfahrt_leave_queue;
static sem_t ausfahrt_sem_left;

static void *ausfahrt_simulator(void *args) {
    while (1) {
        sem_t *sem_complete = call_queue_receive(&ausfahrt_leave_queue);
        sync_printf("A: A car wants to leave!\n");
        
        pthread_mutex_lock(&mutex);
        simulator_state.AA = true;
        pthread_mutex_unlock(&mutex);

        sync_printf("A: Waiting for gate to open...\n");
        pthread_mutex_lock(&mutex);
        while (!simulator_state.A) pthread_cond_wait(&cond_A, &mutex);
        pthread_mutex_unlock(&mutex);

        sync_printf("A: Gate open!\n");
        pthread_mutex_lock(&mutex);
        simulator_state.AA = false;
        pthread_mutex_unlock(&mutex);

        // Notify car that gate has opened
        sem_post(sem_complete);

        sync_printf("A: Waiting for car to drive through...\n");
        sem_wait(&ausfahrt_sem_left);

        sync_printf("A: Car is through!\n");
        pthread_mutex_lock(&mutex);
        simulator_state.AD = false;
        simulator_state.car_cnt--;
        pthread_mutex_unlock(&mutex);

        sync_printf("A: Waiting for gate to close...\n");
        pthread_mutex_lock(&mutex);
        while (simulator_state.A) pthread_cond_wait(&cond_no_A, &mutex);
        pthread_mutex_unlock(&mutex);

        sync_printf("A: Gate closed!\n");
    }
}

static int gen_random(int min, int max) {
    pthread_mutex_lock(&mutex);
    int res = rand_r(&simulator_state.rand_seed);
    pthread_mutex_unlock(&mutex);
    return min + res % (max-min+1);
}

typedef enum {
    DRIVING,
    PARKED
} car_state_t;

static void *car(void *args) {
    car_state_t state = DRIVING;
    int id = args == NULL ? 0 : *((int *)args);
    sem_t enter_complete;
    sem_init(&enter_complete, 0, 0);
    sem_t leave_complete;
    sem_init(&leave_complete, 0, 0);

    while (1) {
        if (state == DRIVING && gen_random(0, 99) < 10) {
            sync_printf("Car %d: wanting to enter.\n", id);
            int ret = call_queue_trycall(&einfahrt_enter_queue, &enter_complete, 30000);
            if (ret) {
                sync_printf("Car %d: giving up.\n", id);
            } else {
                sem_wait(&enter_complete);
                sync_printf("Car %d: driving through gate!.\n", id);
                delay_ms(2000);
                sync_printf("Car %d: went through gate!.\n", id);
                sem_post(&einfahrt_sem_entered);
                sync_printf("Car %d: now parked.\n", id);
                state = PARKED;
            }
        } else if (state == PARKED && gen_random(0, 99) < 3) {
            sync_printf("Car %d: wanting to leave.\n", id);
            call_queue_call(&ausfahrt_leave_queue, &leave_complete);
            sem_wait(&leave_complete);
            sync_printf("Car %d: driving through gate!.\n", id);
            delay_ms(2000);
            sync_printf("Car %d: went through gate!.\n", id);
            sem_post(&ausfahrt_sem_left);
            sync_printf("Car %d: now outside.\n", id);
            state = DRIVING;
        }
        delay_ms(1000);
    }
}

static int thread_args[NUM_CARS];

void init_simulator() {
    pthread_mutex_init(&printf_mutex, NULL);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_E, NULL);
    pthread_cond_init(&cond_A, NULL);
    pthread_cond_init(&cond_no_E, NULL);
    pthread_cond_init(&cond_no_A, NULL);
    srand(time(NULL));

    simulator_state.E = CLOSE;
    simulator_state.A = CLOSE;
    simulator_state.S = FREE;
    simulator_state.EA = false;
    simulator_state.AA = false;
    simulator_state.ED = false;
    simulator_state.AD = false;
    simulator_state.car_cnt = 0;

    call_queue_init(&einfahrt_enter_queue);
    call_queue_init(&ausfahrt_leave_queue);

    pthread_t simulator_thread;
    if (pthread_create(&simulator_thread, NULL, simulator, NULL) != 0) exit(EXIT_FAILURE);

    pthread_t einfahrt_simulator_thread;
    if (pthread_create(&einfahrt_simulator_thread, NULL, einfahrt_simulator, NULL) != 0) exit(EXIT_FAILURE);

    pthread_t ausfahrt_simulator_thread;
    if (pthread_create(&ausfahrt_simulator_thread, NULL, ausfahrt_simulator, NULL) != 0) exit(EXIT_FAILURE);

    int i;
    for (i = 0; i < NUM_CARS; i++) {
        thread_args[i] = i;
        pthread_t car_thread;
        if (pthread_create(&car_thread, NULL, car, &thread_args[i]) != 0) exit(EXIT_FAILURE);
    }
}

void delay_ms(int ms) {
    nanosleep((const struct timespec[]){ms / 1000, (ms % 1000)*1000000L}, NULL);
}

void read_einfahrt_anfrage(bool *EA) {
    pthread_mutex_lock(&mutex);
    *EA = simulator_state.EA;
    pthread_mutex_unlock(&mutex);
}

void read_ausfahrt_anfrage(bool *AA) {
    pthread_mutex_lock(&mutex);
    *AA = simulator_state.AA;
    pthread_mutex_unlock(&mutex);
}

void read_einfahrt_durchfahrt(bool *ED) {
    pthread_mutex_lock(&mutex);
    *ED = simulator_state.ED;
    pthread_mutex_unlock(&mutex);
}

void read_ausfahrt_durchfahrt(bool *AD) {
    pthread_mutex_lock(&mutex);
    *AD = simulator_state.AD;
    pthread_mutex_unlock(&mutex);
}

void write_einfahrt(gate_state_t E) {
    pthread_mutex_lock(&mutex);
    simulator_state.E = E;
    if (E == OPEN) {
        if (simulator_state.EA) {
            simulator_state.ED = true;
        }
        pthread_cond_signal(&cond_E);
    } else {
        pthread_cond_signal(&cond_no_E);
    }
    pthread_mutex_unlock(&mutex);
}

void write_ausfahrt(gate_state_t A) {
    pthread_mutex_lock(&mutex);
    simulator_state.A = A;
    if (A == OPEN) {
        if (simulator_state.AA) {
            simulator_state.AD = true;
        }
        pthread_cond_signal(&cond_A);
    } else {
        pthread_cond_signal(&cond_no_A);
    }
    pthread_mutex_unlock(&mutex);
}

void write_signal(signal_state_t S) {
    pthread_mutex_lock(&mutex);
    simulator_state.S = S;
    pthread_mutex_unlock(&mutex);
}
