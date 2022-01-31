#ifndef CTPOOL_H
#define CTPOOL_H

#include <stdint.h>
#include <pthread.h>

#include "task_queue.h"
#include "thread/thread.h"

typedef enum thread_pool_error {
        thread_pool_invalid          = -1,
        thread_pool_task_invalid     = -2,
        thread_pool_lock_failure     = -3,
        thread_pool_queue_error      = -4,
        thread_pool_shutdown         = -5,
        thread_pool_thread_failure   = -6,
        thread_pool_unsafe_operation = -7
} thread_pool_error_t;

typedef enum thread_pool_destroy_flags {
        thread_pool_immediate_shutdown = 0,  /* try close the pool immediately   */
        thread_pool_graceful_shutdown  = 1   /* wait for threads to finish tasks */
} thread_pool_destroy_flags_t;

/* structure of a thread pool */
typedef struct thread_pool thread_pool_t;
struct thread_pool {
        int8_t          shutdown;      /* Is the current state of the thread pool closed? */
        int32_t         task_count;    /* Number of tasks currently to be run             */
        int32_t         started;       /* Number of threads running                       */
        int32_t         thread_count;  /* Number of threads                               */
        ctpool_cond_t   notify;        /* Conditional variable                            */ 
        ctpool_mutex_t  lock;          /* Mutex                                           */ 
        ctpool_thread_t *threads;      /* Starting Pointer of Thread Array                */
        task_queue_t    *queue;        /* Starting Pointer of Task Queue                  */         
};

thread_pool_t *thread_pool_create(int32_t thread_count);
int32_t thread_pool_add(thread_pool_t *pool, task_fn func, void *arg);
int32_t thread_pool_destroy(thread_pool_t **pool, int32_t flags);

#endif
