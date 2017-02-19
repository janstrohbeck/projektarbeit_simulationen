#ifndef HAVE_CALL_QUEUE_H
#define HAVE_CALL_QUEUE_H

#include <pthread.h>
#include <semaphore.h>

typedef struct entry {
    struct entry *next;
    struct entry *prev;
    pthread_cond_t cond_rendezvous;
    sem_t *cond_complete;
    int received;
} call_queue_entry_t;

typedef struct {
    call_queue_entry_t *first;
    call_queue_entry_t *last;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
} call_queue_t;

void call_queue_init(call_queue_t *queue);
sem_t *call_queue_receive(call_queue_t *queue);
int call_queue_trycall(call_queue_t *queue, sem_t *cond_complete, int timeout_ms);
void call_queue_call(call_queue_t *queue, sem_t *cond_complete);

#endif
