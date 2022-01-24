#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ctpool.h"
#include "clogger.h"

void *thread_log_fn(void *filepath);
void *thread_msg_start_stdout(void *message);
void *thread_show_thread_pool_info(void *pool);
void *thread_msg_finish_stdout(void *message);

int main(void)
{
        thread_pool_t *pool = thread_pool_create(5);
        if (pool == NULL) {
                return EXIT_FAILURE;
        }

        const char *filepath = "logs.txt";
        thread_pool_task_t log_in_logs_txt = thread_pool_task_create(
                thread_log_fn,
                (void *) filepath
        );

        const char *message_start = "***       started      ***";
        thread_pool_task_t message_start_to_stdout = thread_pool_task_create(
                thread_msg_start_stdout,
                (void *) message_start
        );

        thread_pool_task_t show_thread_pool_info = thread_pool_task_create(
                thread_show_thread_pool_info,
                (void *) pool
        );

        const char *message_finish = "*** the queue is empty ***";
        thread_pool_task_t message_finish_to_stdout = thread_pool_task_create(
                thread_msg_finish_stdout,
                (void *) message_finish
        );

        thread_pool_add(pool, &message_start_to_stdout);
        thread_pool_add(pool, &show_thread_pool_info);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &message_finish_to_stdout);

        thread_pool_destroy(pool, thread_pool_graceful_shutdown);
        // thread_pool_destroy(pool, thread_pool_immediate_shutdown);

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
thread_msg_start_stdout(void *message)
{
        const char *msg = message;
        if (msg == NULL) {
                LOG_ERROR("message not provided. msg = (%s)", msg);
                return NULL;
        }

        printf("message of thread: (%ld) >>> %s\n", pthread_self(), msg);

        return NULL;
}

void *
thread_show_thread_pool_info(void *pool)
{
        thread_pool_t *tp = pool;
        if (tp == NULL) {
                LOG_ERROR("thread pool not provided. (%s)", NULL);
                return NULL;
        }

        while (tp->started > 1) {
                printf("queue length %ld - working threads: %d\n",
                       task_queue_length(tp->queue),
                       tp->started);

                       sleep(3);
        }

        return NULL;
}

void *
thread_msg_finish_stdout(void *message)
{
        const char *msg = message;
        if (msg == NULL) {
                LOG_ERROR("message not provided. msg = (%s)", msg);
                return NULL;
        }

        printf("message of thread: (%ld) >>> %s\n", pthread_self(), msg);

        return NULL;
}
