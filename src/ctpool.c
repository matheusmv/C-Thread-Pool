#include "ctpool.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum thread_pool_shutdown {
        immediate_shutdown = 1,
        graceful_shutdown  = 2
};

static void *thread_pool_worker_fn(void *thread_pool);

thread_pool_t *
thread_pool_create(int32_t thread_count)
{
        if (thread_count <= 0) {
                return NULL;
        }

        thread_pool_t *new_pool = malloc(sizeof(thread_pool_t));
        if (new_pool == NULL) {
                fprintf(stderr, "ERROR: failed to create thread pool. (%s)\n",
                        strerror(errno));

                return NULL;
        }

        pthread_t *new_threads = malloc(thread_count * sizeof(pthread_t));
        if (new_threads == NULL) {
                fprintf(stderr, "ERROR: failed to create threads. (%s)\n",
                        strerror(errno));

                free(new_pool);

                return NULL;
        }

        task_queue_t *task_queue = task_queue_create();
        if (task_queue == NULL) {
                free(new_pool);
                free(new_threads);

                return NULL;
        }

        *new_pool = (thread_pool_t) {
                .shutdown     = 0,
                .task_count   = 0,
                .started      = 0,
                .thread_count = 0,
                .queue        = task_queue,
                .threads      = new_threads,
        };

        int32_t error = 0;

        error = ctpool_mutex_init(&new_pool->lock);

        error = ctpool_cond_init(&new_pool->notify);

        for (int32_t i = 0; i < thread_count && !error; ++i) {
                error = ctpool_thread_create(&new_pool->threads[i],
                                              thread_pool_worker_fn,
                                              new_pool);

                new_pool->started += 1;
                new_pool->thread_count += 1;
        }

        if (error) {
                thread_pool_destroy(&new_pool, thread_pool_immediate_shutdown);

                return NULL;
        }

        return new_pool;
}

int32_t
thread_pool_add(thread_pool_t *pool, task_fn func, void *arg)
{
        if (pool == NULL) {
                return thread_pool_invalid;
        }

        if (func == NULL) {
                return thread_pool_task_invalid;
        }

        ctpool_mutex_lock(&pool->lock);

        int32_t error = 0;

        do {
                if (pool->shutdown) {
                        error = thread_pool_shutdown;
                        break;
                }

                if (task_queue_enqueue(pool->queue, func, arg) != 0) {
                        error = thread_pool_queue_error;
                        break;
                }

                pool->task_count += 1;

                if (pool->started < pool->thread_count) {
                        ctpool_cond_signal(&pool->notify);
                }
        } while (0);

        ctpool_mutex_unlock(&pool->lock);

        return error;
}

static int32_t
thread_pool_free(thread_pool_t **pool)
{
        if (pool == NULL || *pool == NULL) {
                return thread_pool_invalid;
        }

        if ((*pool)->started > 0) {
                ctpool_mutex_unlock(&(*pool)->lock);
                return thread_pool_unsafe_operation;
        }

        if ((*pool)->threads != NULL) {
                free((*pool)->threads);
                (*pool)->threads = NULL;

                if ((*pool)->queue != NULL) {
                        task_queue_free(&(*pool)->queue);
                }

                ctpool_mutex_destroy(&(*pool)->lock);
                ctpool_cond_destroy(&(*pool)->notify);
        }

        free(*pool);
        *pool = NULL;

        return 0;
}

int32_t
thread_pool_destroy(thread_pool_t **pool, int32_t flags)
{
        if (pool == NULL || *pool == NULL) {
                return thread_pool_invalid;
        }

        ctpool_mutex_lock(&(*pool)->lock);

        int32_t error = 0;

        do {
                if ((*pool)->shutdown) {
                        error = thread_pool_shutdown;
                        break;
                }

                (*pool)->shutdown = (flags & thread_pool_graceful_shutdown) ?
                          graceful_shutdown : immediate_shutdown;

                ctpool_cond_broadcast(&(*pool)->notify);
                ctpool_mutex_unlock(&(*pool)->lock);

                for (int32_t i = 0; i < (*pool)->thread_count; ++i) {
                        if (ctpool_thread_join(&(*pool)->threads[i]) != 0) {
                                error = thread_pool_thread_failure;
                        }
                }
        } while (0);

        if (!error) {
                error = thread_pool_free(pool);
        }

        return error;
}

static inline int32_t
there_are_no_tasks_to_process(thread_pool_t *pool)
{
        return pool->shutdown == immediate_shutdown ||
               (pool->shutdown == graceful_shutdown &&
                pool->task_count == 0); 
}

static void *
thread_pool_worker_fn(void *thread_pool)
{
        assert(thread_pool != NULL);

        thread_pool_t *pool = thread_pool;
        task_t *task;

        for (;;) {
                ctpool_mutex_lock(&pool->lock);

                while ((pool->task_count == 0) && (!pool->shutdown)) {
                        pool->started -= 1;
                        ctpool_cond_wait(&pool->notify, &pool->lock);
                        pool->started += 1;
                }

                if (there_are_no_tasks_to_process(pool)) {
                            break;
                }

                task = task_queue_dequeue(pool->queue);
                pool->task_count -= 1;

                ctpool_mutex_unlock(&pool->lock);

                task_execute(&task);
        }

        pool->started -= 1;

        ctpool_mutex_unlock(&pool->lock);

        return NULL;
}
