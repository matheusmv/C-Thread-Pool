#include "thread.h"

int32_t
ctpool_thread_create(ctpool_thread_t *tid, thread_fn func, void *arg)
{
        return pthread_create(tid, NULL, func, arg);
}

int32_t
ctpool_thread_join(ctpool_thread_t *tid)
{
        return pthread_join(*tid, NULL);
}

int32_t
ctpool_mutex_init(ctpool_mutex_t *mutex)
{
        return pthread_mutex_init(mutex, NULL);
}

void
ctpool_mutex_lock(ctpool_mutex_t *mutex)
{
        pthread_mutex_lock(mutex);
}

void
ctpool_mutex_unlock(ctpool_mutex_t *mutex)
{
        pthread_mutex_unlock(mutex);
}

void
ctpool_mutex_destroy(ctpool_mutex_t *mutex)
{
        pthread_mutex_destroy(mutex);
}

void
ctpool_cond_broadcast(ctpool_cond_t *cond)
{
        pthread_cond_broadcast(cond);
}

void
ctpool_cond_destroy(ctpool_cond_t *cond)
{
        pthread_cond_destroy(cond);
}

int32_t
ctpool_cond_init(ctpool_cond_t *cond)
{
        return pthread_cond_init(cond, NULL);
}

void
ctpool_cond_signal(ctpool_cond_t *cond)
{
        pthread_cond_signal(cond);
}

void
ctpool_cond_wait(ctpool_cond_t *cond, ctpool_mutex_t *mutex)
{
        pthread_cond_wait(cond, mutex);
}
