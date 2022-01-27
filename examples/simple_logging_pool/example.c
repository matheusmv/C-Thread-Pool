#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../src/ctpool.h"
#include "../../src/clogger.h"

/* arg for 'thread_show_thread_pool_info' */
typedef struct tp_id {
        char          *name;
        thread_pool_t *pool;
} tp_id_t;

void *thread_log_fn(void *filepath);
void *thread_show_thread_pool_info(void *pool);

int main(void)
{
        thread_pool_t *pool1 = thread_pool_create(5);

        /* args */
        tp_id_t id_pool1 = {  .name = "pool1", .pool = pool1 };
        char *filepath1 = "logs1.txt";
        char *filepath2 = "logs2.txt";

        /* adding tasks to thread pool */
        /* tasks are waiting in the task queue */

        thread_pool_add(pool1, thread_show_thread_pool_info, &id_pool1);
        thread_pool_add(pool1, thread_log_fn, filepath1);
        thread_pool_add(pool1, thread_log_fn, filepath2);
        thread_pool_add(pool1, thread_log_fn, filepath1);
        thread_pool_add(pool1, thread_log_fn, filepath2);
        thread_pool_add(pool1, thread_log_fn, filepath1);
        thread_pool_add(pool1, thread_log_fn, filepath2);
        thread_pool_add(pool1, thread_log_fn, filepath1);
        thread_pool_add(pool1, thread_log_fn, filepath2);
        thread_pool_add(pool1, thread_log_fn, filepath1);
        thread_pool_add(pool1, thread_log_fn, filepath2);

        /* try close the pool immediately */
        // thread_pool_destroy(&pool1, thread_pool_immediate_shutdown);

        /* wait for threads to finish tasks */
        thread_pool_destroy(&pool1, thread_pool_graceful_shutdown);

        assert(pool1 == NULL);

        return EXIT_SUCCESS;
}

void *
thread_log_fn(void *filepath)
{
        const char *fp = filepath;
        if (fp == NULL) {
                LOG_ERROR("file not provided. f = (%s)", fp);
                return NULL;
        }

        const int counter = 10;
        for (int i = 0; i < counter; i++) {
                LOG_INFO_F(fp, "%d info message of thread %ld", i, pthread_self());
        }

        return NULL;
}

void *
thread_show_thread_pool_info(void *pool)
{
        tp_id_t *id = pool;
        if (id == NULL) {
                LOG_ERROR("id not provided. (%s)", NULL);
                return NULL;
        }

        LOG_DEBUG("POOL: %s starting", id->name);

        while (id->pool->started > 1) {
                printf("POOL: %s INFO >>> remaining tasks: %d - working threads: %d\n",
                       id->name,
                       id->pool->task_count,
                       id->pool->started);

                       sleep(3);
        }

        LOG_DEBUG("POOL: %s finished", id->name);

        return NULL;
}
