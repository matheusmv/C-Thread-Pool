#ifndef CTPOOL_H
#define CTPOOL_H

#include <stdint.h>
#include <pthread.h>

#include "task_queue.h"

#define true  1
#define false 0

typedef enum thread_pool_error {
        thread_pool_invalid        = -1,
        thread_pool_lock_failure   = -2,
        thread_pool_queue_error    = -3,
        thread_pool_shutdown       = -4,
        thread_pool_thread_failure = -5
} thread_pool_error_t;

typedef enum thread_pool_destroy_flags {
        thread_pool_graceful = 1
} thread_pool_destroy_flags_t;

/* structure of a thread pool */
typedef struct thread_pool thread_pool_t;
struct thread_pool {
        int8_t          shutdown;      /* Is the current state of the thread pool closed? */
        int32_t         started;       /* Number of threads running                       */
        int32_t         thread_count;  /* Number of threads                               */
        pthread_t       *threads;      /* Starting Pointer of Thread Array                */
        task_queue_t    *queue;        /* Starting Pointer of Task Queue Array            */
        pthread_cond_t  notify;        /* Conditional variable                            */
        pthread_mutex_t mutex;         /* mutex                                           */
};

thread_pool_t *thread_pool_create(int32_t thread_count);
int thread_pool_add(thread_pool_t *pool, thread_pool_task_t *task);
int thread_pool_destroy(thread_pool_t *pool, int flags);

#endif
