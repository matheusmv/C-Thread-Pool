#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <stddef.h>
#include <pthread.h>

typedef void *(*thread_fn)(void *);

/* Task to be executed */
typedef struct Task Task_t;
struct Task {
        thread_fn function;  /* function to be run         */
        void      *argument; /* parameters of the function */
        Task_t    *next;     /* pointer to the next task   */
};

/* Queue of tasks to be executed */
typedef struct task_queue task_queue_t;
struct task_queue {
        Task_t          *head;  /* head of task queue */
        Task_t          *tail;  /* tail of task queue */
        size_t          length; /* task queue length  */
        pthread_mutex_t lock;   /* queue mutex        */
};

Task_t task_create(thread_fn function, void *argument);
void *task_execute(Task_t *task);

task_queue_t *task_queue_create(void);
int task_queue_enqueue(task_queue_t *queue, Task_t *task);
int task_queue_dequeue(task_queue_t *queue, Task_t *task);
void task_queue_free(task_queue_t *queue);

#endif
