#include "task_queue.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
task_queue_mutex_init(task_queue_t *queue)
{
        pthread_mutex_init(&queue->mutex, NULL);
}

static void
task_queue_mutex_lock(task_queue_t *queue)
{
        pthread_mutex_lock(&queue->mutex);
}

static void
task_queue_mutex_unlock(task_queue_t *queue)
{
        pthread_mutex_unlock(&queue->mutex);
}

static void
task_queue_mutex_destroy(task_queue_t *queue)
{
        pthread_mutex_destroy(&queue->mutex);
}

thread_pool_task_t
thread_pool_task_create(thread_fn function, void *argument)
{
        return (thread_pool_task_t) {
                .argument        = argument,
                .routine         = function,
                .next            = NULL,
                .prev            = NULL,
        };
}

static void
increase_queue_length(task_queue_t *queue)
{
        queue->length += 1;
}

static void
decrease_queue_length(task_queue_t *queue)
{
        if (queue->length > 0) {
                queue->length -= 1;
        }
}

task_queue_t *
task_queue_create(void)
{
        task_queue_t *queue = malloc(sizeof(task_queue_t));
        if (queue == NULL) {
                fprintf(stderr, "ERROR: failed to create task queue. (%s)\n",
                        strerror(errno));

                return NULL;
        }

        *queue = (task_queue_t) {
                .head   = NULL,
                .tail   = NULL,
                .length = 0
        };

        task_queue_mutex_init(queue);

        return queue;
}

size_t
task_queue_length(task_queue_t *queue)
{
        return queue->length;
}

int
task_queue_is_empty(task_queue_t *queue)
{
        return queue->tail == NULL;
}

int
task_queue_enqueue(task_queue_t *queue, thread_pool_task_t *task)
{
        assert(queue != NULL && task != NULL);

        task_queue_mutex_lock(queue);

        thread_pool_task_t *new_task = malloc(sizeof(thread_pool_task_t));
        if (new_task == NULL) {
                fprintf(stderr, "ERROR: failed to create new task. (%s)\n",
                        strerror(errno));

                task_queue_mutex_unlock(queue);

                return -1;
        }

        thread_pool_task_t *tail = queue->tail;

        if (!task_queue_is_empty(queue)) {
                tail->next = new_task;
        }

        *new_task = (thread_pool_task_t) {
                .argument        = task->argument,
                .routine         = task->routine,
                .prev            = tail,
                .next            = NULL,
        };

        if (queue->head == NULL) {
                queue->head = new_task;
        }

        queue->tail = new_task;

        increase_queue_length(queue);

        task_queue_mutex_unlock(queue);

        return 0;
}

int
task_queue_dequeue(task_queue_t *queue, thread_pool_task_t *dest)
{
        assert(queue != NULL);

        task_queue_mutex_lock(queue);

        if(!task_queue_is_empty(queue)) {
                thread_pool_task_t *head = queue->head;

                if(task_queue_length(queue) == 1) {
                        queue->head = NULL;
                        queue->tail = NULL;
                } else {
                        queue->head = head->next;
                        queue->head->prev = NULL;
                }

                if (dest != NULL && head != NULL) {
                        memmove(dest, head, sizeof(thread_pool_task_t));
                }

                decrease_queue_length(queue);
                free(head);

                task_queue_mutex_unlock(queue);

                return 0;
        }

        task_queue_mutex_unlock(queue);

        return -1;
}

int
task_queue_peek(task_queue_t *queue, thread_pool_task_t *dest)
{
        assert(queue != NULL);

        task_queue_mutex_lock(queue);

        if (!task_queue_is_empty(queue) && dest != NULL) {
                memmove(dest, queue->head, sizeof(thread_pool_task_t));

                task_queue_mutex_unlock(queue);

                return 0;
        }

        task_queue_mutex_unlock(queue);

        return -1;
}

void
task_queue_free(task_queue_t *queue)
{
        if (queue != NULL) {
                while(!task_queue_is_empty(queue)) {
                        task_queue_dequeue(queue, NULL);
                }

                task_queue_mutex_destroy(queue);

                free(queue);
                queue = NULL;
        }
}
