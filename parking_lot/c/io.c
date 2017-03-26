#include "io.h"
#include "request_queue.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdarg.h>

#define NUM_CARS 20

// The mutex for the simulator state
static pthread_mutex_t mutex;
// Condition variables (for waiting for gate states)
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

// Mutex for sync_printf
static pthread_mutex_t printf_mutex;
// Allow only one thread at a time to write to the console
int sync_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    int ret = vprintf(format, args);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
    return ret;
}

// Initialize the random generator
static int simulator_state_init_random() {
    srand(time(NULL));
}

// Get the next random integer between 0 and 99
static int simulator_state_get_random() {
    pthread_mutex_lock(&mutex);
    int res = rand_r(&simulator_state.rand_seed);
    pthread_mutex_unlock(&mutex);
    static const int min = 0;
    static const int max = 99;
    return min + res % (max-min+1);
}

// The possible states that a car can be in
typedef enum {
    DRIVING,
    PARKED
} car_state_t;

// The following functions are all getters and setters for the simulation state.

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
    // Set entry sensor to blocked immediately after the gate is opened.
    // This allows the main program to block immediately after opening the
    // gate, waiting for the sensor to be free again.
    // Also, signal condition variables
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
    // Set exit sensor to blocked immediately after the gate is opened.
    // This allows the main program to block immediately after opening the
    // gate, waiting for the sensor to be free again.
    // Also, signal condition variables
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

// A thread that prints the current state of the simulation when the state changes.
static void *logger(void *args) {
    // Flag that is used to print the state on first start of the task.
    bool start = true;
    
    // Variables to save previous values of the state members in order to
    // check if something changed.
    signal_state_t prev_signal_state = SIGNAL_FREE;
    int prev_car_cnt = 0;
    int prev_entry_queue_cnt = 0;
    int prev_exit_queue_cnt = 0;

    while (1) {
        // The current values of the state members
        signal_state_t tmp_signal_state = simulator_state_get_signal_state();
        int tmp_car_cnt = simulator_state_get_car_cnt();
        int tmp_entry_queue_cnt = simulator_state_get_entry_queue_cnt();
        int tmp_exit_queue_cnt = simulator_state_get_exit_queue_cnt();

        // Check if something changed
        if (start ||
                tmp_signal_state != prev_signal_state ||
                tmp_car_cnt != prev_car_cnt ||
                tmp_entry_queue_cnt != prev_entry_queue_cnt ||
                tmp_exit_queue_cnt != prev_exit_queue_cnt) {
            // print current state
            sync_printf("Cars inside: %d | Signal: %s | Cars wanting to enter: %d | Cars wanting to leave: %d\n",
                    tmp_car_cnt, tmp_signal_state == SIGNAL_FREE ? "Free" : "Full",
                    tmp_entry_queue_cnt, tmp_exit_queue_cnt);
            start = false;
            // save current state values for comparison later
            prev_signal_state = tmp_signal_state;
            prev_car_cnt = tmp_car_cnt;
            prev_entry_queue_cnt = tmp_entry_queue_cnt;
            prev_exit_queue_cnt = tmp_exit_queue_cnt;
        }
        delay_ms(100);
    }
    return NULL;
}

// Log only if it is configured, prepend everything with "EntryGate: "
#if DISPLAY_GATE_STATE
#define LOG_ENTRY_GATE(message) sync_printf("EntryGate: %s\n", (message))
#else
// Make logging a no-op
#define LOG_ENTRY_GATE(message) ((void)0)
#endif

// Request queue for the entry gate
static request_queue_t entry_gate_request_queue;
// Semaphore that is used by cars to signal the entry gate simulator when they
// have finished entering the gate.
static sem_t entry_gate_sem_entered;

static void *entry_gate_simulator(void *args) {
    while (1) {
        // Wait for a car to try to enter
        sem_t *sem_complete = request_queue_receive(&entry_gate_request_queue);
        LOG_ENTRY_GATE("A car wants to enter!");
        
        simulator_state_set_entry_request(true);

        LOG_ENTRY_GATE("Waiting for gate to open...");
        simulator_state_wait_entry_gate_open();

        LOG_ENTRY_GATE("Gate open!");
        simulator_state_set_entry_request(false);
        simulator_state_inc_car_cnt();

        // Notify car that gate has opened
        sem_post(sem_complete);

        LOG_ENTRY_GATE("Waiting for car to drive through...");
        // Wait for car to signal us that it has finished entering
        sem_wait(&entry_gate_sem_entered);

        LOG_ENTRY_GATE("Car is through!");
        simulator_state_set_entry_sensor_state(false);

        LOG_ENTRY_GATE("Waiting for gate to close...");
        simulator_state_wait_entry_gate_closed();

        LOG_ENTRY_GATE("Gate closed!");
    }
    return NULL;
}

// Log only if it is configured, prepend everything with "EntryGate: "
#if DISPLAY_GATE_STATE
#define LOG_EXIT_GATE(message) sync_printf("ExitGate: %s\n", (message))
#else
// Make logging a no-op
#define LOG_EXIT_GATE(message) ((void)0)
#endif

// Request queue for the exit gate
static request_queue_t exit_gate_request_queue;
// Semaphore that is used by cars to signal the exit gate simulator when they
// have finished leaving the gate.
static sem_t exit_gate_sem_left;

static void *exit_gate_simulator(void *args) {
    while (1) {
        sem_t *sem_complete = request_queue_receive(&exit_gate_request_queue);
        LOG_EXIT_GATE("A car wants to leave!");
        
        simulator_state_set_exit_request(true);

        LOG_EXIT_GATE("Waiting for gate to open...");
        simulator_state_wait_exit_gate_open();

        LOG_EXIT_GATE("Gate open!");
        simulator_state_set_exit_request(false);
        simulator_state_dec_car_cnt();

        // Notify car that gate has opened
        sem_post(sem_complete);

        LOG_EXIT_GATE("Waiting for car to drive through...");
        // Wait for car to signal us that it has finished leaving
        sem_wait(&exit_gate_sem_left);

        LOG_EXIT_GATE("Car is through!");
        simulator_state_set_exit_sensor_state(false);

        LOG_EXIT_GATE("Waiting for gate to close...");
        simulator_state_wait_exit_gate_closed();

        LOG_EXIT_GATE("Gate closed!");
    }
    return NULL;
}

// Log only if it is configured, prepend everything with "EntryGate: "
#if DISPLAY_CAR_STATE
#define LOG_CAR(id, message) sync_printf("Car %d: %s\n", (id), (message))
#else
// Make logging a no-op
#define LOG_CAR(id, message) ((void)0)
#endif

static void *car(void *args) {
    car_state_t state = DRIVING;
    int id = args == NULL ? 0 : *((int *)args);
    (void)id;
    // Two semaphores that will be given to the gate simulator threads
    // using the request queues
    sem_t enter_complete;
    sem_init(&enter_complete, 0, 0);
    sem_t leave_complete;
    sem_init(&leave_complete, 0, 0);

    while (1) {
        // With 10% probability, try to enter the parking lot
        if (state == DRIVING && simulator_state_get_random(0, 99) < 10) {
            LOG_CAR(id, "wanting to enter.");
            simulator_state_inc_entry_queue_cnt();
            // Try to enter, but abort if the simulator task did not accept it
            // in 30 seconds
            int ret = request_queue_tryenqueue(&entry_gate_request_queue, &enter_complete, 30000);
            simulator_state_dec_entry_queue_cnt();
            if (ret) {
                LOG_CAR(id, "giving up.");
            } else {
                // Wait for the entry gate thread to signal us that the gate is open
                sem_wait(&enter_complete);
                LOG_CAR(id, "driving through gate!");
                delay_ms(2000);
                LOG_CAR(id, "went through gate!");
                // Signal the entry gate thread that we have have completed
                // driving through the gate
                sem_post(&entry_gate_sem_entered);
                LOG_CAR(id, "now parked.");
                state = PARKED;
            }
        // With 3% probability, try to leave the parking lot
        } else if (state == PARKED && simulator_state_get_random(0, 99) < 3) {
            LOG_CAR(id, "wanting to leave.");
            simulator_state_inc_exit_queue_cnt();
            // Enqueue to leave the lot, no abort in this case
            request_queue_enqueue(&exit_gate_request_queue, &leave_complete);
            simulator_state_dec_exit_queue_cnt();
            // Wait for the exit gate simulator to signal us that the gate is open
            sem_wait(&leave_complete);
            LOG_CAR(id, "driving through gate!");
            delay_ms(2000);
            LOG_CAR(id, "went through gate!");
            // Signal the exit gate thread that we have completed driving
            // through the gate
            sem_post(&exit_gate_sem_left);
            LOG_CAR(id, "now outside.");
            state = DRIVING;
        }
        delay_ms(1000);
    }
    return NULL;
}

// Saves the arguments for the car threads (the ids)
static int thread_args[NUM_CARS];

void init_simulator() {
    pthread_mutex_init(&printf_mutex, NULL);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_entry_gate_open, NULL);
    pthread_cond_init(&cond_exit_gate_open, NULL);
    pthread_cond_init(&cond_entry_gate_closed, NULL);
    pthread_cond_init(&cond_exit_gate_closed, NULL);

    sem_init(&entry_gate_sem_entered, 0, 0);
    sem_init(&exit_gate_sem_left, 0, 0);

    // Initialize simulator state
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

    simulator_state_init_random();

    request_queue_init(&entry_gate_request_queue);
    request_queue_init(&exit_gate_request_queue);

    // Create all threads

    pthread_t logger_thread;
    if (pthread_create(&logger_thread, NULL, logger, NULL) != 0) exit(EXIT_FAILURE);

    pthread_t entry_gate_simulator_thread;
    if (pthread_create(&entry_gate_simulator_thread, NULL, entry_gate_simulator, NULL) != 0) exit(EXIT_FAILURE);

    pthread_t exit_gate_simulator_thread;
    if (pthread_create(&exit_gate_simulator_thread, NULL, exit_gate_simulator, NULL) != 0) exit(EXIT_FAILURE);

    // Create car threads. Each threads received a pointer an integer
    // containing its id
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

// The following functions just read from the state

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
