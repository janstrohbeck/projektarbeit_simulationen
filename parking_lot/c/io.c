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
static pthread_cond_t cond_entry_gate_open;
static pthread_cond_t cond_exit_gate_open;
static pthread_cond_t cond_entry_gate_closed;
static pthread_cond_t cond_exit_gate_closed;

static struct {
    unsigned int rand_seed;
    gate_state_t entry_gate_state;
    gate_state_t exit_gate_state;
    signal_state_t signal_state;
    bool entry_request;
    bool exit_request;
    bool entry_sensor_state;
    bool exit_sensor_state;
    int car_cnt;
    int entry_queue_cnt;
    int exit_queue_cnt;
} simulator_state;

static pthread_mutex_t printf_mutex;
int sync_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    int ret = vprintf(format, args);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
    return ret;
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

static bool simulator_state_get_entry_request() {
    bool ER;
    pthread_mutex_lock(&mutex);
    ER = simulator_state.entry_request;
    pthread_mutex_unlock(&mutex);
    return ER;
}

static void simulator_state_set_entry_request(bool ER) {
    pthread_mutex_lock(&mutex);
    simulator_state.entry_request = ER;
    pthread_mutex_unlock(&mutex);
}

static bool simulator_state_get_exit_request() {
    bool ER;
    pthread_mutex_lock(&mutex);
    ER = simulator_state.exit_request;
    pthread_mutex_unlock(&mutex);
    return ER;
}

static void simulator_state_set_exit_request(bool ER) {
    pthread_mutex_lock(&mutex);
    simulator_state.exit_request = ER;
    pthread_mutex_unlock(&mutex);
}

static gate_state_t simulator_state_get_entry_gate_state() {
    gate_state_t E;
    pthread_mutex_lock(&mutex);
    E = simulator_state.entry_gate_state;
    pthread_mutex_unlock(&mutex);
    return E;
}

static void simulator_state_set_entry_gate_state(gate_state_t E) {
    pthread_mutex_lock(&mutex);
    simulator_state.entry_gate_state = E;
    if (E == GATE_OPEN) {
        if (simulator_state.entry_request) {
            simulator_state.entry_sensor_state = SENSOR_BLOCKED;
        }
        pthread_cond_signal(&cond_entry_gate_open);
    } else {
        pthread_cond_signal(&cond_entry_gate_closed);
    }
    pthread_mutex_unlock(&mutex);
}

static gate_state_t simulator_state_get_exit_gate_state() {
    gate_state_t E;
    pthread_mutex_lock(&mutex);
    E = simulator_state.exit_gate_state;
    pthread_mutex_unlock(&mutex);
    return E;
}

static void simulator_state_set_exit_gate_state(gate_state_t E) {
    pthread_mutex_lock(&mutex);
    simulator_state.exit_gate_state = E;
    if (E == GATE_OPEN) {
        if (simulator_state.exit_request) {
            simulator_state.exit_sensor_state = SENSOR_BLOCKED;
        }
        pthread_cond_signal(&cond_exit_gate_open);
    } else {
        pthread_cond_signal(&cond_exit_gate_closed);
    }
    pthread_mutex_unlock(&mutex);
}

static sensor_state_t simulator_state_get_entry_sensor_state() {
    sensor_state_t ESS;
    pthread_mutex_lock(&mutex);
    ESS = simulator_state.entry_sensor_state;
    pthread_mutex_unlock(&mutex);
    return ESS;
}

static void simulator_state_set_entry_sensor_state(sensor_state_t ESS) {
    pthread_mutex_lock(&mutex);
    simulator_state.entry_sensor_state = ESS;
    pthread_mutex_unlock(&mutex);
}

static sensor_state_t simulator_state_get_exit_sensor_state() {
    sensor_state_t ESS;
    pthread_mutex_lock(&mutex);
    ESS = simulator_state.exit_sensor_state;
    pthread_mutex_unlock(&mutex);
    return ESS;
}

static void simulator_state_set_exit_sensor_state(sensor_state_t ESS) {
    pthread_mutex_lock(&mutex);
    simulator_state.exit_sensor_state = ESS;
    pthread_mutex_unlock(&mutex);
}

static signal_state_t simulator_state_get_signal_state() {
    signal_state_t S;
    pthread_mutex_lock(&mutex);
    S = simulator_state.signal_state;
    pthread_mutex_unlock(&mutex);
    return S;
}

static void simulator_state_set_signal_state(signal_state_t S) {
    pthread_mutex_lock(&mutex);
    simulator_state.signal_state = S;
    pthread_mutex_unlock(&mutex);
}

static int simulator_state_get_car_cnt() {
    int car_cnt;
    pthread_mutex_lock(&mutex);
    car_cnt = simulator_state.car_cnt;
    pthread_mutex_unlock(&mutex);
    return car_cnt;
}

static void simulator_state_inc_car_cnt() {
    pthread_mutex_lock(&mutex);
    simulator_state.car_cnt++;
    pthread_mutex_unlock(&mutex);
}

static void simulator_state_dec_car_cnt() {
    pthread_mutex_lock(&mutex);
    simulator_state.car_cnt--;
    pthread_mutex_unlock(&mutex);
}

static int simulator_state_get_entry_queue_cnt() {
    int entry_queue_cnt;
    pthread_mutex_lock(&mutex);
    entry_queue_cnt = simulator_state.entry_queue_cnt;
    pthread_mutex_unlock(&mutex);
    return entry_queue_cnt;
}

static void simulator_state_inc_entry_queue_cnt() {
    pthread_mutex_lock(&mutex);
    simulator_state.entry_queue_cnt++;
    pthread_mutex_unlock(&mutex);
}

static void simulator_state_dec_entry_queue_cnt() {
    pthread_mutex_lock(&mutex);
    simulator_state.entry_queue_cnt--;
    pthread_mutex_unlock(&mutex);
}

static int simulator_state_get_exit_queue_cnt() {
    int exit_queue_cnt;
    pthread_mutex_lock(&mutex);
    exit_queue_cnt = simulator_state.exit_queue_cnt;
    pthread_mutex_unlock(&mutex);
    return exit_queue_cnt;
}

static void simulator_state_inc_exit_queue_cnt() {
    pthread_mutex_lock(&mutex);
    simulator_state.exit_queue_cnt++;
    pthread_mutex_unlock(&mutex);
}

static void simulator_state_dec_exit_queue_cnt() {
    pthread_mutex_lock(&mutex);
    simulator_state.exit_queue_cnt--;
    pthread_mutex_unlock(&mutex);
}

static void simulator_state_wait_entry_gate_open() {
    pthread_mutex_lock(&mutex);
    while (simulator_state.entry_gate_state != GATE_OPEN) pthread_cond_wait(&cond_entry_gate_open, &mutex);
    pthread_mutex_unlock(&mutex);
}

static void simulator_state_wait_entry_gate_closed() {
    pthread_mutex_lock(&mutex);
    while (simulator_state.entry_gate_state != GATE_CLOSED) pthread_cond_wait(&cond_entry_gate_closed, &mutex);
    pthread_mutex_unlock(&mutex);
}

static void simulator_state_wait_exit_gate_open() {
    pthread_mutex_lock(&mutex);
    while (simulator_state.exit_gate_state != GATE_OPEN) pthread_cond_wait(&cond_exit_gate_open, &mutex);
    pthread_mutex_unlock(&mutex);
}

static void simulator_state_wait_exit_gate_closed() {
    pthread_mutex_lock(&mutex);
    while (simulator_state.exit_gate_state != GATE_CLOSED) pthread_cond_wait(&cond_exit_gate_closed, &mutex);
    pthread_mutex_unlock(&mutex);
}

static void *logger(void *args) {
    bool start = true;
    
    signal_state_t prev_signal_state = SIGNAL_FREE;
    int prev_car_cnt = 0;
    int prev_entry_queue_cnt = 0;
    int prev_exit_queue_cnt = 0;

    signal_state_t tmp_signal_state;
    int tmp_car_cnt;
    int tmp_entry_queue_cnt;
    int tmp_exit_queue_cnt;

    while (1) {
        tmp_signal_state = simulator_state_get_signal_state();
        tmp_car_cnt = simulator_state_get_car_cnt();
        tmp_entry_queue_cnt = simulator_state_get_entry_queue_cnt();
        tmp_exit_queue_cnt = simulator_state_get_exit_queue_cnt();

        if (start ||
                tmp_signal_state != prev_signal_state ||
                tmp_car_cnt != prev_car_cnt ||
                tmp_entry_queue_cnt != prev_entry_queue_cnt ||
                tmp_exit_queue_cnt != prev_exit_queue_cnt) {
            sync_printf("Cars inside: %d | Signal: %s | Cars wanting to enter: %d | Cars wanting to leave: %d\n",
                    tmp_car_cnt, tmp_signal_state == SIGNAL_FREE ? "Free" : "Full",
                    tmp_entry_queue_cnt, tmp_exit_queue_cnt);
            start = false;
            prev_signal_state = tmp_signal_state;
            prev_car_cnt = tmp_car_cnt;
            prev_entry_queue_cnt = tmp_entry_queue_cnt;
            prev_exit_queue_cnt = tmp_exit_queue_cnt;
        }
        delay_ms(100);
    }
    return NULL;
}

static call_queue_t entry_gate_request_queue;
static sem_t entry_gate_sem_entered;

static void *entry_gate_simulator(void *args) {
    while (1) {
        sem_t *sem_complete = call_queue_receive(&entry_gate_request_queue);
        sync_printf("EntryGate: A car wants to enter!\n");
        
        simulator_state_set_entry_request(true);

        sync_printf("EntryGate: Waiting for gate to open...\n");
        simulator_state_wait_entry_gate_open();

        sync_printf("EntryGate: Gate open!\n");
        simulator_state_set_entry_request(false);
        simulator_state_inc_car_cnt();

        // Notify car that gate has opened
        sem_post(sem_complete);

        sync_printf("EntryGate: Waiting for car to drive through...\n");
        sem_wait(&entry_gate_sem_entered);

        sync_printf("EntryGate: Car is through!\n");
        simulator_state_set_entry_sensor_state(false);

        sync_printf("EntryGate: Waiting for gate to close...\n");
        simulator_state_wait_entry_gate_closed();

        sync_printf("EntryGate: Gate closed!\n");
    }
    return NULL;
}

static call_queue_t exit_gate_request_queue;
static sem_t exit_gate_sem_left;

static void *exit_gate_simulator(void *args) {
    while (1) {
        sem_t *sem_complete = call_queue_receive(&exit_gate_request_queue);
        sync_printf("ExitGate: A car wants to leave!\n");
        
        simulator_state_set_exit_request(true);

        sync_printf("ExitGate: Waiting for gate to open...\n");
        simulator_state_wait_exit_gate_open();

        sync_printf("ExitGate: Gate open!\n");
        simulator_state_set_exit_request(false);
        simulator_state_dec_car_cnt();

        // Notify car that gate has opened
        sem_post(sem_complete);

        sync_printf("ExitGate: Waiting for car to drive through...\n");
        sem_wait(&exit_gate_sem_left);

        sync_printf("ExitGate: Car is through!\n");
        simulator_state_set_exit_sensor_state(false);

        sync_printf("ExitGate: Waiting for gate to close...\n");
        simulator_state_wait_exit_gate_closed();

        sync_printf("ExitGate: Gate closed!\n");
    }
    return NULL;
}

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
            simulator_state_inc_entry_queue_cnt();
            int ret = call_queue_trycall(&entry_gate_request_queue, &enter_complete, 30000);
            simulator_state_dec_entry_queue_cnt();
            if (ret) {
                sync_printf("Car %d: giving up.\n", id);
            } else {
                sem_wait(&enter_complete);
                sync_printf("Car %d: driving through gate!.\n", id);
                delay_ms(2000);
                sync_printf("Car %d: went through gate!.\n", id);
                sem_post(&entry_gate_sem_entered);
                sync_printf("Car %d: now parked.\n", id);
                state = PARKED;
            }
        } else if (state == PARKED && gen_random(0, 99) < 3) {
            sync_printf("Car %d: wanting to leave.\n", id);
            simulator_state_inc_exit_queue_cnt();
            call_queue_call(&exit_gate_request_queue, &leave_complete);
            simulator_state_dec_exit_queue_cnt();
            sem_wait(&leave_complete);
            sync_printf("Car %d: driving through gate!.\n", id);
            delay_ms(2000);
            sync_printf("Car %d: went through gate!.\n", id);
            sem_post(&exit_gate_sem_left);
            sync_printf("Car %d: now outside.\n", id);
            state = DRIVING;
        }
        delay_ms(1000);
    }
    return NULL;
}

static int thread_args[NUM_CARS];

void init_simulator() {
    pthread_mutex_init(&printf_mutex, NULL);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_entry_gate_open, NULL);
    pthread_cond_init(&cond_exit_gate_open, NULL);
    pthread_cond_init(&cond_entry_gate_closed, NULL);
    pthread_cond_init(&cond_exit_gate_closed, NULL);
    srand(time(NULL));

    sem_init(&entry_gate_sem_entered, 0, 0);
    sem_init(&exit_gate_sem_left, 0, 0);

    simulator_state.entry_gate_state = GATE_CLOSED;
    simulator_state.exit_gate_state = GATE_CLOSED;
    simulator_state.signal_state = SIGNAL_FREE;
    simulator_state.entry_request = false;
    simulator_state.exit_request = false;
    simulator_state.entry_sensor_state = SENSOR_FREE;
    simulator_state.exit_sensor_state = SENSOR_FREE;
    simulator_state.car_cnt = 0;
    simulator_state.entry_queue_cnt = 0;
    simulator_state.exit_queue_cnt = 0;

    call_queue_init(&entry_gate_request_queue);
    call_queue_init(&exit_gate_request_queue);

    pthread_t logger_thread;
    if (pthread_create(&logger_thread, NULL, logger, NULL) != 0) exit(EXIT_FAILURE);

    pthread_t entry_gate_simulator_thread;
    if (pthread_create(&entry_gate_simulator_thread, NULL, entry_gate_simulator, NULL) != 0) exit(EXIT_FAILURE);

    pthread_t exit_gate_simulator_thread;
    if (pthread_create(&exit_gate_simulator_thread, NULL, exit_gate_simulator, NULL) != 0) exit(EXIT_FAILURE);

    pthread_t car_threads[NUM_CARS];
    int i;
    for (i = 0; i < NUM_CARS; i++) {
        thread_args[i] = i;
        if (pthread_create(&car_threads[i], NULL, car, &thread_args[i]) != 0) exit(EXIT_FAILURE);
    }
}

void delay_ms(int ms) {
    nanosleep((const struct timespec[]){{ms / 1000, (ms % 1000)*1000000L}}, NULL);
}

void read_entry_request(bool *ER) {
    *ER = simulator_state_get_entry_request();
}

void read_exit_request(bool *ER) {
    *ER = simulator_state_get_exit_request();
}

void read_entry_sensor_state(bool *ESS) {
    *ESS = simulator_state_get_entry_sensor_state();
}

void read_exit_sensor_state(bool *ESS) {
    *ESS = simulator_state_get_exit_sensor_state();
}

void write_entry_gate_state(gate_state_t E) {
    simulator_state_set_entry_gate_state(E);
}

void write_exit_gate_state(gate_state_t E) {
    simulator_state_set_exit_gate_state(E);
}

void write_signal_state(signal_state_t S) {
    simulator_state_set_signal_state(S);
}
