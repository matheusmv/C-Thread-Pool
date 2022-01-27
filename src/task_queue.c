#include "task_queue.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
task_queue_lock(task_queue_t *queue)
{
        pthread_mutex_lock(&queue->lock);
}

static void
task_queue_unlock(task_queue_t *queue)
{
        pthread_mutex_unlock(&queue->lock);
}

task_t
task_create(task_fn function, void *argument)
{
        return (task_t) {
                .function = function,
                .argument = argument,
                .next     = NULL,
        };
}

void *
task_execute(task_t *task)
{
        if (task == NULL) {
                return NULL;
        }

        void *result = NULL;

        result = task->function(task->argument);

        return result;
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

        pthread_mutex_init(&queue->lock, NULL);

        return queue;
}

static inline size_t
task_queue_length(task_queue_t *queue)
{
        return queue->length;
}

static inline int32_t
task_queue_is_empty(task_queue_t *queue)
{
        return queue->tail == NULL;
}

int32_t
task_queue_enqueue(task_queue_t *queue, task_t *task)
{
        if (queue == NULL || task == NULL) {
                return -1;
        }

        task_t *new_task = malloc(sizeof(task_t));
        if (new_task == NULL) {
                return -1;
        }

        *new_task = (task_t) {
                .function = task->function,
                .argument = task->argument,
                .next     = NULL,
        };

        task_queue_lock(queue);

        if (queue->head == NULL) {
                queue->head = new_task;
        }

        if (!task_queue_is_empty(queue)) {
                queue->tail->next = new_task;
        }

        queue->tail = new_task;

        increase_queue_length(queue);

        task_queue_unlock(queue);

        return 0;
}

int32_t
task_queue_dequeue(task_queue_t *queue, task_t *dest)
{
        if (queue == NULL) {
                return -1;
        }

        task_queue_lock(queue);

        if(!task_queue_is_empty(queue)) {
                task_t *head = queue->head;

                if(task_queue_length(queue) == 1) {
                        queue->head = NULL;
                        queue->tail = NULL;
                } else {
                        queue->head = head->next;
                }

                if (dest != NULL && head != NULL) {
                        memmove(dest, head, sizeof(task_t));
                }

                decrease_queue_length(queue);

                task_queue_unlock(queue);

                free(head);

                return 0;
        }

        task_queue_unlock(queue);

        return -1;
}

void
task_queue_free(task_queue_t **queue)
{
        if (queue != NULL && *queue != NULL) {
                task_queue_lock(*queue);

                task_t *head = (*queue)->head;
                while(head != NULL) {
                        task_t *task = head;
                        head = head->next;
                        free(task);
                }

                (*queue)->tail = NULL;
                (*queue)->length = 0;

                pthread_mutex_destroy(&(*queue)->lock);

                free(*queue);
                *queue = NULL;
        }
}
