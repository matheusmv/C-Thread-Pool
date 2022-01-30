#include "ctpool.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum thread_pool_shutdown {
        immediate_shutdown = 1,
        graceful_shutdown  = 2
};

static int32_t
thread_pool_mutex_lock(thread_pool_t *pool)
{
        int32_t status = 0;

        status = pthread_mutex_lock(&pool->lock);

        return status;
}

static int32_t
thread_pool_mutex_unlock(thread_pool_t *pool)
{
        int32_t status = 0;

        status = pthread_mutex_unlock(&pool->lock);

        return status;
}

static int32_t
thread_pool_cond_signal(thread_pool_t *pool)
{
        int32_t status = 0;

        status = pthread_cond_signal(&pool->notify);

        return status;
}

static int32_t
thread_pool_cond_wait(thread_pool_t *pool)
{
        int32_t status = 0;

        status = pthread_cond_wait(&pool->notify, &pool->lock);

        return status;
}

static int32_t
thread_pool_cond_broadcast(thread_pool_t *pool)
{
        int32_t status = 0;

        status = pthread_cond_broadcast(&pool->notify);

        return status;
}

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

        int32_t status = 0;

        status = pthread_mutex_init(&new_pool->lock, NULL);
        if (status != 0) {
                fprintf(stderr, "ERROR: failed to create thread pool. (%d)\n",
                        status);

                free(new_pool);
                free(new_threads);
                task_queue_free(&task_queue);

                return NULL;
        }

        status = pthread_cond_init(&new_pool->notify, NULL);
        if (status != 0) {
                fprintf(stderr, "ERROR: failed to create thread pool. (%d)\n",
                        status);

                free(new_pool);
                free(new_threads);
                task_queue_free(&task_queue);
                pthread_mutex_destroy(&new_pool->lock);

                return NULL;
        }

        for (int32_t i = 0; i < thread_count; ++i) {
                status = pthread_create(&new_pool->threads[i], NULL,
                                        thread_pool_worker_fn,
                                        (void *) new_pool);

                new_pool->started += 1;
                new_pool->thread_count += 1;
        }

        if (status != 0) {
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

        if (thread_pool_mutex_lock(pool) != 0) {
                return thread_pool_lock_failure;
        }

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

                if (thread_pool_cond_signal(pool) != 0) {
                        error = thread_pool_lock_failure;
                        break;
                }
        } while (0);

        if (thread_pool_mutex_unlock(pool) != 0) {
                return thread_pool_lock_failure;
        }

        return error;
}

static int32_t
thread_pool_free(thread_pool_t **pool)
{
        if (pool == NULL || *pool == NULL) {
                return thread_pool_invalid;
        }

        if ((*pool)->started > 0) {
                thread_pool_mutex_unlock(*pool);
                return thread_pool_unsafe_operation;
        }

        if ((*pool)->threads != NULL) {
                free((*pool)->threads);
                (*pool)->threads = NULL;

                if ((*pool)->queue != NULL) {
                        task_queue_free(&(*pool)->queue);
                }

                pthread_mutex_destroy(&(*pool)->lock);
                pthread_cond_destroy(&(*pool)->notify);
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

        if (thread_pool_mutex_lock(*pool) != 0) {
                return thread_pool_lock_failure;
        }

        int32_t error = 0;

        do {
                if ((*pool)->shutdown) {
                        error = thread_pool_shutdown;
                        break;
                }

                (*pool)->shutdown = (flags & thread_pool_graceful_shutdown) ?
                          graceful_shutdown : immediate_shutdown;

                if ((thread_pool_cond_broadcast(*pool) != 0) ||
                    (thread_pool_mutex_unlock(*pool) != 0)) {
                        error = thread_pool_lock_failure;
                        break;
                }

                for (int32_t i = 0; i < (*pool)->thread_count; ++i) {
                        if (pthread_join((*pool)->threads[i], NULL) != 0) {
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
                thread_pool_mutex_lock(pool);

                while ((pool->task_count == 0) && (!pool->shutdown)) {
                        pool->started -= 1;
                        thread_pool_cond_wait(pool);
                        pool->started += 1;
                }

                if (there_are_no_tasks_to_process(pool)) {
                            break;
                }

                task = task_queue_dequeue(pool->queue);
                pool->task_count -= 1;

                thread_pool_mutex_unlock(pool);

                task_execute(&task);
        }

        pool->started -= 1;

        thread_pool_mutex_unlock(pool);

        return NULL;
}
