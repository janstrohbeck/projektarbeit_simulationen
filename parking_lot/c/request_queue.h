#ifndef HAVE_REQUEST_QUEUE_H
#define HAVE_REQUEST_QUEUE_H

#include <pthread.h>
#include <semaphore.h>

// An entry of the request queue.
typedef struct entry {
    // Pointer to the next element in the queue (or NULL).
    struct entry *next;
    // Pointer to the previous element in the queue (or NULL).
    struct entry *prev;
    // Pointer to a condition variable that is signaled by the queue when the
    // entry was received by a thread. The requesting thread will then be resumed.
    pthread_cond_t cond_rendezvous;
    // A pointer to a semaphore that can be signaled by the receiving thread to
    // notify the requesting thread that its request has been processed. A
    // semaphore is used instead of a condition variable as the two threads do
    // not have access to the queue’s mutex.
    sem_t *cond_complete;
    // A flag that will be set by the queue when the entry has been received by
    // a thread. It is used to check if a request was received or if a timeout
    // was the cause why a thread was resumed.
    int received;
} request_queue_entry_t;

// The basic structure of the request queue.
typedef struct {
    // Pointer to the first entry in the queue (or NULL).
    request_queue_entry_t *first;
    // Pointer to the last entry in the queue (or NULL).
    request_queue_entry_t *last;
    // A mutex which is used to only allow one thread at a time to use the queue.
    pthread_mutex_t mutex;
    // A condition variable which is signaled when a request is placed in the
    // queue. This allows receiving threads to be resumed when new entries are
    // in the queue.
    pthread_cond_t cond_not_empty;
} request_queue_t;

// Initialize a new request queue
void request_queue_init(request_queue_t *queue);

// Blocks the calling thread until a request can be received from the queue.
sem_t *request_queue_receive(request_queue_t *queue);

// Adds a request to the queue and blocks the calling thread until the request
// was received by a thread or the specified timeout duration was exceeded
// (given in milliseconds). The calling thread has to provide a pointer to a
// semaphore which can be signaled by the receiving thread when the request was
// processed.
int request_queue_tryenqueue(request_queue_t *queue, sem_t *cond_complete, int timeout_ms);

// Adds a request to the queue and blocks the calling thread until the request
// was received by a thread. The calling thread has to provide a pointer to a
// semaphore which can be signaled by the receiving thread when the request was
// processed.
void request_queue_enqueue(request_queue_t *queue, sem_t *cond_complete);

#endif
