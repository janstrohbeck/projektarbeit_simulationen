#include "request_queue.h"
#include <stdlib.h>
#include <malloc.h>

void request_queue_init(request_queue_t * const queue) {
    queue->first = NULL;
    queue->last = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_not_empty, NULL);
}

sem_t *request_queue_receive(request_queue_t * const queue) {
    pthread_mutex_lock(&queue->mutex);

    // wait for something to be in the queue
    while (queue->first == NULL) pthread_cond_wait(&queue->cond_not_empty, &queue->mutex);

    request_queue_entry_t *entry = queue->first;
    // mark the entry as received
    entry->received = 1;
    // remove it from the queue
    queue->first = entry->next;
    if (entry == queue->last) {
        queue->last = NULL;
    }

    sem_t *cond_complete = entry->cond_complete;

    // signal the requesting thread that its request was received
    pthread_cond_signal(&entry->cond_rendezvous);

    pthread_mutex_unlock(&queue->mutex);

    // return the semaphore from the requesting thread, so that the receiving
    // thread can signal it when it has processed the requesting thread
    return cond_complete;
}

// Creates a timespec that contains a time that is ms milliseconds in the future.
static struct timespec timeInTheFuture(int ms) {
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    now.tv_sec += ms / 1000;
    now.tv_nsec += 1000000 * (ms % 1000);
    now.tv_sec += now.tv_nsec / (1000000000);
    now.tv_nsec %= (1000000000);
    return now;
}

int request_queue_tryenqueue(request_queue_t * const queue, sem_t * const cond_complete, const int timeout_ms) {
    struct timespec timeout = timeInTheFuture(timeout_ms);
    // Create a new entry struct
    request_queue_entry_t *entry = (request_queue_entry_t *)malloc(sizeof(request_queue_entry_t));

    pthread_cond_init(&entry->cond_rendezvous, NULL);
    entry->cond_complete = cond_complete;
    entry->prev = NULL;
    entry->next = NULL;
    entry->received = 0;

    pthread_mutex_lock(&queue->mutex);

    // Add entry to the end of the queue
    if (queue->last != NULL) {
        entry->prev = queue->last->prev;
        queue->last->next = entry;
    }
    queue->last = entry;

    if (queue->first == NULL) {
        queue->first = entry;
    }

    int res = 0;
    // Signal receiving threads that something is in the queue
    pthread_cond_signal(&queue->cond_not_empty);
    // If we got no timeout, block until the entry was received
    if (timeout_ms < 0) {
        while (!entry->received) pthread_cond_wait(&entry->cond_rendezvous, &queue->mutex);
    // if we got a timeout duration, wait only until that time has elapsed
    } else {
        res = pthread_cond_timedwait(&entry->cond_rendezvous, &queue->mutex, &timeout);

        // If we were not received, we must remove ourselves from the queue
        // otherwise, the receiving thread has already removed us from the queue
        if (res != 0 && !entry->received) {
            // remove entry from queue
            if (entry->prev != NULL && entry->next != NULL) {
                entry->prev->next = entry->next;
                entry->next->prev = entry->prev;
            } else if (entry->prev != NULL) {
                entry->prev->next = NULL;
                queue->last = entry->prev;
            } else if (entry->next != NULL) {
                entry->next->prev = NULL;
                queue->first = entry->next;
            } else {
                queue->first = NULL;
                queue->last = NULL;
            }
        }
    }

    // The entry must be freed in any case
    free(entry);

    pthread_mutex_unlock(&queue->mutex);

    return res;
}

void request_queue_enqueue(request_queue_t * const queue, sem_t * const cond_complete) {
    request_queue_tryenqueue(queue, cond_complete, -1);
}
