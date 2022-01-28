#include "task.h"

#include <stdlib.h>

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
