#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../src/ctpool.h"
#include "../../src/clogger.h"

#define ARR_LEN(ARR) (sizeof(ARR) / sizeof(ARR)[0])

/* arg for 'thread_show_thread_pool_info' */
typedef struct tp_id {
        char          *name;
        thread_pool_t *pool;
} tp_id_t;

void *thread_log_fn(void *filepath);
void *thread_show_thread_pool_info(void *pool);

/* just to keep the main function clean */

void add_tasks_on_pool1(thread_pool_t *pool);
void add_tasks_on_pool2(thread_pool_t *pool);
void add_tasks_on_pool3(thread_pool_t *pool);

int main(void)
{
        thread_pool_t *pools[3];

        pools[0] = thread_pool_create(2);
        pools[1] = thread_pool_create(6);
        pools[2] = thread_pool_create(4);

        // /* creating the tasks */
        tp_id_t id_pool1 = {  .name = "pool1", .pool = pools[0] };
        Task_t show_thread_pool1_info = task_create(
                thread_show_thread_pool_info,
                (void *) &id_pool1
        );

        thread_pool_add(pools[0], &show_thread_pool1_info);
        add_tasks_on_pool1(pools[0]);

        tp_id_t id_pool2 = {  .name = "pool2", .pool = pools[1] };
        Task_t show_thread_pool2_info = task_create(
                thread_show_thread_pool_info,
                (void *) &id_pool2
        );

        thread_pool_add(pools[1], &show_thread_pool2_info);
        add_tasks_on_pool2(pools[1]);

        tp_id_t id_pool3 = {  .name = "pool3", .pool = pools[2] };
        Task_t show_thread_pool3_info = task_create(
                thread_show_thread_pool_info,
                (void *) &id_pool3
        );

        thread_pool_add(pools[2], &show_thread_pool3_info);
        add_tasks_on_pool3(pools[2]);

        /* try close the pool immediately */
        // for (size_t i = 0; i < ARR_LEN(pools); ++i) {
        //         thread_pool_destroy(pools[i], thread_pool_immediate_shutdown);
        // }

        /* wait for threads to finish tasks */
        for (size_t i = 0; i < ARR_LEN(pools); ++i) {
                thread_pool_destroy(pools[i], thread_pool_graceful_shutdown);
        }

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
add_tasks_on_pool1(thread_pool_t *pool)
{
        char *filepath1 = "logs1.txt";
        Task_t log_in_logs1_txt = task_create(
                thread_log_fn,
                (void *) filepath1
        );

        char *filepath2 = "logs2.txt";
        Task_t log_in_logs2_txt = task_create(
                thread_log_fn,
                (void *) filepath2
        );

        thread_pool_add(pool, &log_in_logs1_txt);
        thread_pool_add(pool, &log_in_logs2_txt);
}

void
add_tasks_on_pool2(thread_pool_t *pool)
{
        char *filepath1 = "logs3.txt";
        Task_t log_in_logs3_txt = task_create(
                thread_log_fn,
                (void *) filepath1
        );

        char *filepath2 = "logs4.txt";
        Task_t log_in_logs4_txt = task_create(
                thread_log_fn,
                (void *) filepath2
        );

        thread_pool_add(pool, &log_in_logs3_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs3_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs3_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs3_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs3_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
        thread_pool_add(pool, &log_in_logs3_txt);
        thread_pool_add(pool, &log_in_logs4_txt);
}

void
add_tasks_on_pool3(thread_pool_t *pool)
{
        char *filepath1 = "logs5.txt";
        Task_t log_in_logs5_txt = task_create(
                thread_log_fn,
                (void *) filepath1
        );

        char *filepath2 = "logs6.txt";
        Task_t log_in_logs6_txt = task_create(
                thread_log_fn,
                (void *) filepath2
        );

        thread_pool_add(pool, &log_in_logs5_txt);
        thread_pool_add(pool, &log_in_logs6_txt);
        thread_pool_add(pool, &log_in_logs5_txt);
        thread_pool_add(pool, &log_in_logs6_txt);
        thread_pool_add(pool, &log_in_logs6_txt);
}
