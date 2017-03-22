#ifndef HAVE_REQUEST_QUEUE_H
#define HAVE_REQUEST_QUEUE_H

#include <pthread.h>
#include <semaphore.h>

typedef struct entry {
    struct entry *next;
    struct entry *prev;
    pthread_cond_t cond_rendezvous;
    sem_t *cond_complete;
    int received;
} request_queue_entry_t;

typedef struct {
    request_queue_entry_t *first;
    request_queue_entry_t *last;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
} request_queue_t;

void request_queue_init(request_queue_t *queue);
sem_t *request_queue_receive(request_queue_t *queue);
int request_queue_tryenqueue(request_queue_t *queue, sem_t *cond_complete, int timeout_ms);
void request_queue_enqueue(request_queue_t *queue, sem_t *cond_complete);

#endif
