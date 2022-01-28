#ifndef TASK_H
#define TASK_H

typedef void *(*task_fn)(void *);

/* task to be executed */
typedef struct task task_t;
struct task {
        task_fn function;  /* function to be run         */
        void    *argument; /* parameters of the function */
        task_t  *next;     /* pointer to the next task   */
};

task_t *task_create(task_fn function, void *argument);
void *task_execute(task_t **task);

#endif /* TASK_H */
