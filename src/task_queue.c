#include "task_queue.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

task_t *
task_create(task_fn function, void *argument)
{
        task_t *new_task = malloc(sizeof(task_t));
        if (new_task == NULL) {
                return NULL;
        }

        *new_task = (task_t) {
                .function = function,
                .argument = argument,
                .next     = NULL,
        };

        return new_task;
}

void *
task_execute(task_t **task)
{
        if (task == NULL || *task == NULL) {
                return NULL;
        }

        void *result = NULL;
        task_fn func = (*task)->function;
        void *arg    = (*task)->argument;

        result = func(arg);

        free(*task);
        *task = NULL;

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
task_queue_enqueue(task_queue_t *queue, task_fn func, void *arg)
{
        if (queue == NULL || func == NULL) {
                return -1;
        }

        task_t *new_task = task_create(func, arg);
        if (new_task == NULL) {
                return -1;
        }

        if (queue->head == NULL) {
                queue->head = new_task;
        }

        if (!task_queue_is_empty(queue)) {
                queue->tail->next = new_task;
        }

        queue->tail = new_task;

        increase_queue_length(queue);

        return 0;
}

task_t *
task_queue_dequeue(task_queue_t *queue)
{
        if (queue == NULL) {
                return NULL;
        }

        if(!task_queue_is_empty(queue)) {
                task_t *head = queue->head;

                if(task_queue_length(queue) == 1) {
                        queue->head = NULL;
                        queue->tail = NULL;
                } else {
                        queue->head = head->next;
                }

                decrease_queue_length(queue);

                return head;
        }

        return NULL;
}

void
task_queue_free(task_queue_t **queue)
{
        if (queue != NULL && *queue != NULL) {

                task_t *head = (*queue)->head;
                while(head != NULL) {
                        task_t *task = head;
                        head = head->next;
                        free(task);
                }

                (*queue)->tail = NULL;
                (*queue)->length = 0;

                free(*queue);
                *queue = NULL;
        }
}
