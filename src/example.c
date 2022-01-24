#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ctpool.h"
#include "clogger.h"

void *thread_log_fn(void *filepath);
void *thread_msg_stdout(void *message);

int main(void)
{
        const char *filepath = "logs.txt";
        const char *message = "programming in C is fun";

        thread_pool_t *pool = thread_pool_create(5);
        if (pool == NULL) {
                return EXIT_FAILURE;
        }

        thread_pool_task_t log_in_logs_txt = thread_pool_task_create(thread_log_fn, (void *) filepath);
        thread_pool_task_t message_to_stdout = thread_pool_task_create(thread_msg_stdout, (void *) message);

        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &message_to_stdout);
        thread_pool_add(pool, &message_to_stdout);
        thread_pool_add(pool, &log_in_logs_txt);
        thread_pool_add(pool, &log_in_logs_txt);

        thread_pool_destroy(pool, thread_pool_graceful);

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
                sleep(3);
        }

        return NULL;
}

void *
thread_msg_stdout(void *message)
{
        const char *msg = message;
        if (msg == NULL) {
                LOG_ERROR("message not provided. msg = (%s)", msg);
                return NULL;
        }

        const int counter = 10;
        for (int i = 0; i < counter; i++) {
                printf("%d - message of thread: (%ld) >>> %s\n", i, pthread_self(), msg);
                sleep(2);
        }

        return NULL;
}
