#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../src/ctpool.h"
#include "../../src/clogger.h"

#define ARR_LEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

/* arg for 'thread_show_thread_pool_info' */
typedef struct tp_id {
        char          *name;
        thread_pool_t *pool;
} tp_id_t;

void *thread_log_fn(void *filepath);
void *thread_show_thread_pool_info(void *pool);

/* just to keep the main function clean */

void add_tasks_on_pool1(thread_pool_t *pool, tp_id_t *pool_id);
void add_tasks_on_pool2(thread_pool_t *pool, tp_id_t *pool_id);
void add_tasks_on_pool3(thread_pool_t *pool, tp_id_t *pool_id);

int main(void)
{
        thread_pool_t *pools[3];

        pools[0] = thread_pool_create(2);
        pools[1] = thread_pool_create(6);
        pools[2] = thread_pool_create(4);

        // /* creating the tasks */
        tp_id_t id_pool1 = {  .name = "pool1", .pool = pools[0] };
        add_tasks_on_pool1(pools[0], &id_pool1);

        tp_id_t id_pool2 = {  .name = "pool2", .pool = pools[1] };
        add_tasks_on_pool2(pools[1], &id_pool2);

        tp_id_t id_pool3 = {  .name = "pool3", .pool = pools[2] };
        add_tasks_on_pool3(pools[2], &id_pool3);

        /* try close the pool immediately */
        // for (size_t i = 0; i < ARR_LEN(pools); ++i) {
        //         thread_pool_destroy(&pools[i], thread_pool_immediate_shutdown);
        // }

        /* wait for threads to finish tasks */
        for (size_t i = 0; i < ARR_LEN(pools); ++i) {
                thread_pool_destroy(&pools[i], thread_pool_graceful_shutdown);
        }

        assert(pools[0] == NULL);
        assert(pools[1] == NULL);
        assert(pools[2] == NULL);

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

                       sleep(2);
        }

        LOG_DEBUG("POOL: %s finished", id->name);

        return NULL;
}

void
add_tasks_on_pool1(thread_pool_t *pool, tp_id_t *pool_id)
{
        thread_pool_add(pool, thread_show_thread_pool_info, pool_id);
        thread_pool_add(pool, thread_log_fn, "logs1.txt");
        thread_pool_add(pool, thread_log_fn, "logs2.txt");
}

void
add_tasks_on_pool2(thread_pool_t *pool, tp_id_t *pool_id)
{
        thread_pool_add(pool, thread_show_thread_pool_info, pool_id);
        thread_pool_add(pool, thread_log_fn, "logs3.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs3.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs3.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs3.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs3.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
        thread_pool_add(pool, thread_log_fn, "logs3.txt");
        thread_pool_add(pool, thread_log_fn, "logs4.txt");
}

void
add_tasks_on_pool3(thread_pool_t *pool, tp_id_t *pool_id)
{
        thread_pool_add(pool, thread_show_thread_pool_info, pool_id);
        thread_pool_add(pool, thread_log_fn, "logs5.txt");
        thread_pool_add(pool, thread_log_fn, "logs6.txt");
        thread_pool_add(pool, thread_log_fn, "logs5.txt");
        thread_pool_add(pool, thread_log_fn, "logs6.txt");
        thread_pool_add(pool, thread_log_fn, "logs6.txt");
}
