#ifndef WAVEMUX_THREAD_H
#define WAVEMUX_THREAD_H

#include "fifo.h"

typedef struct wavemux_thread_t {
    pthread_t thread;
    pthread_mutex_t mtx;
    int socket;
    FIFO *mailbox;
    struct wavemux_thread_t *next;
} wavemux_thread_t;

typedef void (*wavemux_thread_iter_t)(wavemux_thread_t *thr, void *arg);

void wavemux_thread_init(void *(*start_routine) (void *), int socket);
void wavemux_thread_deinit(wavemux_thread_t *thr);
void wavemux_thread_pool_add(wavemux_thread_t *thr);
void wavemux_thread_pool_remove(wavemux_thread_t *thr);
void wavemux_thread_pool_each(wavemux_thread_iter_t func, void *arg);

#endif
