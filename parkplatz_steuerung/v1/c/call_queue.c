#include "call_queue.h"
#include <stdlib.h>

void call_queue_init(call_queue_t * const queue) {
    queue->first = NULL;
    queue->last = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_not_empty, NULL);
}

sem_t *call_queue_receive(call_queue_t * const queue) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->first == NULL) pthread_cond_wait(&queue->cond_not_empty, &queue->mutex);

    call_queue_entry_t *entry = queue->first;
    entry->received = 1;
    queue->first = entry->next;

    sem_t *cond_complete = entry->cond_complete;

    pthread_cond_signal(&entry->cond_rendezvous);

    pthread_mutex_unlock(&queue->mutex);

    return cond_complete;
}

static struct timespec timeInTheFuture(int ms) {
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    now.tv_sec += ms / 1000;
    now.tv_nsec += 1000000 * (ms % 1000);
    now.tv_sec += now.tv_nsec / (1000000000);
    now.tv_nsec %= (1000000000);
    return now;
}

int call_queue_trycall(call_queue_t * const queue, sem_t * const cond_complete, const int timeout_ms) {
    struct timespec timeout = timeInTheFuture(timeout_ms);
    call_queue_entry_t entry;

    pthread_cond_init(&entry.cond_rendezvous, NULL);
    entry.cond_complete = cond_complete;
    entry.prev = NULL;
    entry.next = NULL;
    entry.received = 0;

    pthread_mutex_lock(&queue->mutex);

    if (queue->last != NULL) {
        entry.prev = queue->last->prev;
        queue->last->next = &entry;
    }
    queue->last = &entry;

    if (queue -> first == NULL) {
        queue->first = &entry;
    }

    int res = 0;
    pthread_cond_signal(&queue->cond_not_empty);
    if (timeout_ms < 0) {
        while (!entry.received) pthread_cond_wait(&entry.cond_rendezvous, &queue->mutex);
    }
    else {
        res = pthread_cond_timedwait(&entry.cond_rendezvous, &queue->mutex, &timeout);

        if (res != 0 && !entry.received) {
            // remove entry from queue
            if (entry.prev != NULL && entry.next != NULL) {
                entry.prev->next = entry.next;
                entry.next->prev = entry.prev;
            } else if (entry.prev != NULL) {
                entry.prev->next = NULL;
                queue->last = entry.prev;
            } else if (entry.next != NULL) {
                entry.next->prev = NULL;
                queue->first = entry.next;
            } else {
                queue->first = NULL;
                queue->last = NULL;
            }
        }
    }

    pthread_mutex_unlock(&queue->mutex);

    return res;
}

void call_queue_call(call_queue_t * const queue, sem_t * const cond_complete) {
    call_queue_trycall(queue, cond_complete, -1);
}
