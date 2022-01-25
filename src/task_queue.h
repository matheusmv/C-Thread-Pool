#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <stddef.h>
#include <pthread.h>

typedef void *(*thread_fn)(void *);

/* Task to be executed */
typedef struct thread_pool_task thread_pool_task_t;
struct thread_pool_task {
        thread_fn          function;      /* function to be run         */
        void               *argument;     /* parameters of the function */
        thread_pool_task_t *next;         /* pointer to the next task   */
};

/* Queue of tasks to be executed */
typedef struct task_queue task_queue_t;
struct task_queue {
        thread_pool_task_t *head;         /* head of task queue */
        thread_pool_task_t *tail;         /* tail of task queue */
        size_t             length;        /* task queue length  */
        pthread_mutex_t    lock;          /* queue mutex        */
};

thread_pool_task_t thread_pool_task_create(thread_fn function, void *argument);

task_queue_t *task_queue_create(void);
int task_queue_enqueue(task_queue_t *queue, thread_pool_task_t *task);
int task_queue_dequeue(task_queue_t *queue, thread_pool_task_t *task);
void task_queue_free(task_queue_t *queue);

#endif
