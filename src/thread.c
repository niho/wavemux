#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "thread.h"

static wavemux_thread_t *threads = NULL;
static pthread_mutex_t threads_mutex = PTHREAD_MUTEX_INITIALIZER;


void wavemux_thread_init(void *(*start_routine) (void *), int socket) {
    wavemux_thread_t *thr;

    thr = (wavemux_thread_t *)malloc(sizeof(wavemux_thread_t));

    thr->socket = socket;
    thr->mailbox = (FIFO *)malloc(sizeof(FIFO));
    thr->next = NULL;

    wavemux_fifo_init(thr->mailbox, 64, sizeof(int));

    if (pthread_create(&thr->thread, NULL, start_routine, (void*)thr))
    {
        perror("pthread create failed");
        return;
    }

    wavemux_thread_pool_add(thr);
}

void wavemux_thread_deinit(wavemux_thread_t *thr) {
    if (thr->mailbox) {
        wavemux_fifo_deinit(thr->mailbox);
        free(thr->mailbox);
    }
    memset(thr, 0, sizeof(wavemux_thread_t));
}

void wavemux_thread_pool_add(wavemux_thread_t *thr) {
    pthread_mutex_lock(&threads_mutex);

    if (threads) {
        wavemux_thread_t *t = threads;
        while(1) {
            if (t->next == NULL) {
                t->next = thr;
                break;
            } else {
                t = t->next;
            }
        }
    } else {
        threads = thr;
    }

    pthread_mutex_unlock(&threads_mutex);
}

void wavemux_thread_pool_remove(wavemux_thread_t *thr) {
    pthread_mutex_lock(&threads_mutex);

    if (threads == thr) {
        threads = thr->next;
        return;
    }

    if (threads) {
        wavemux_thread_t *t = threads;
        while(1) {
            if (t->next == thr) {
                t->next = thr->next;
                break;
            } else {
                t = t->next;
            }
        }
    }

    pthread_mutex_unlock(&threads_mutex);
}

void wavemux_thread_pool_each(wavemux_thread_iter_t func, void *arg) {
    pthread_mutex_lock(&threads_mutex);

    if (threads == NULL)
        return;

    wavemux_thread_t *t = threads;
    while(1) {
        func(t, arg);
        if (t->next)
            t = t->next;
        else
            break;
    }

    pthread_mutex_unlock(&threads_mutex);
}

#if 0
static void wavemux_thread_send(wavemux_thread_t *thr, void *msg) {
    printf("send: %#x %#x\n", thr, msg);
    wavemux_fifo_push(thr->mailbox, msg);
}

static void wavemux_thread_recv(wavemux_thread_t *thr, void **msg) {
    wavemux_fifo_pop(thr->mailbox, msg);
}

static void wavemux_thread_pool_send_all(void *msg) {
    wavemux_thread_pool_each(wavemux_thread_send, msg);
}
#endif

