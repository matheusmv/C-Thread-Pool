#ifndef TASK_QUEUE_H
#define tASK_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

typedef void *(*task_fn)(void *);

/* task to be executed */
typedef struct task task_t;
struct task {
        task_fn function;  /* function to be run         */
        void    *argument; /* parameters of the function */
        task_t  *next;     /* pointer to the next task   */
};

/* Queue of tasks to be executed */
typedef struct task_queue task_queue_t;
struct task_queue {
        task_t          *head;  /* head of task queue */
        task_t          *tail;  /* tail of task queue */
        size_t          length; /* task queue length  */
};

task_t task_create(task_fn function, void *argument);
void *task_execute(task_t *task);

task_queue_t *task_queue_create(void);
int32_t task_queue_enqueue(task_queue_t *queue, task_t *task);
int32_t task_queue_dequeue(task_queue_t *queue, task_t *task);
void task_queue_free(task_queue_t **queue);

#endif
