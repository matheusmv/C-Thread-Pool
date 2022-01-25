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

        /* creating the tasks */

        tp_id_t id_pool1 = {  .name = "pool1", .pool = pool1 };
        thread_pool_task_t show_thread_pool_info = thread_pool_task_create(
                thread_show_thread_pool_info,
                (void *) &id_pool1
        );

        char *filepath1 = "logs1.txt";
        thread_pool_task_t log_in_logs1_txt = thread_pool_task_create(
                thread_log_fn,
                (void *) filepath1
        );

        char *filepath2 = "logs2.txt";
        thread_pool_task_t log_in_logs2_txt = thread_pool_task_create(
                thread_log_fn,
                (void *) filepath2
        );

        /* adding tasks to thread pool */
        /* tasks are waiting in the task queue */

        LOG_DEBUG("Thread %s starting", id_pool1.name);

        thread_pool_add(pool1, &show_thread_pool_info);
        thread_pool_add(pool1, &log_in_logs1_txt);
        thread_pool_add(pool1, &log_in_logs2_txt);
        thread_pool_add(pool1, &log_in_logs1_txt);
        thread_pool_add(pool1, &log_in_logs2_txt);
        thread_pool_add(pool1, &log_in_logs1_txt);
        thread_pool_add(pool1, &log_in_logs2_txt);
        thread_pool_add(pool1, &log_in_logs1_txt);
        thread_pool_add(pool1, &log_in_logs2_txt);
        thread_pool_add(pool1, &log_in_logs1_txt);
        thread_pool_add(pool1, &log_in_logs2_txt);

        /* try close the pool immediately */
        // thread_pool_destroy(pool1, thread_pool_immediate_shutdown);

        /* wait for threads to finish tasks */
        thread_pool_destroy(pool1, thread_pool_graceful_shutdown);

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
                sleep(2);
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

        while (id->pool->started > 1) {
                printf("POOL: %s INFO >>> queue length %ld - working threads: %d\n",
                       id->name,
                       task_queue_length(id->pool->queue),
                       id->pool->started);

                       sleep(3);
        }

        LOG_DEBUG("POOL: %s finished", id->name);

        return NULL;
}
