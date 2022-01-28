#ifndef TASK_QUEUE_H
#define tASK_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#include "task.h"

/* Queue of tasks to be executed */
typedef struct task_queue task_queue_t;
struct task_queue {
        task_t *head;   /* head of task queue */
        task_t *tail;   /* tail of task queue */
        size_t length;  /* task queue length  */
};

task_queue_t *task_queue_create(void);
int32_t task_queue_enqueue(task_queue_t *queue, task_fn func, void *arg);
task_t *task_queue_dequeue(task_queue_t *queue);
void task_queue_free(task_queue_t **queue);

#endif
