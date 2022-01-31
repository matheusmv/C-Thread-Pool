#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>
#include <stdint.h>

typedef pthread_t ctpool_thread_t;
typedef pthread_mutex_t ctpool_mutex_t;
typedef pthread_cond_t ctpool_cond_t;

typedef void *(*thread_fn)(void *arg);

int32_t ctpool_thread_create(ctpool_thread_t *tid, thread_fn func, void *arg);
int32_t ctpool_thread_join(ctpool_thread_t *tid);

int32_t ctpool_mutex_init(ctpool_mutex_t *mutex);
void ctpool_mutex_lock(ctpool_mutex_t *mutex);
void ctpool_mutex_unlock(ctpool_mutex_t *mutex);
void ctpool_mutex_destroy(ctpool_mutex_t *mutex);

void ctpool_cond_broadcast(ctpool_cond_t *cond);
void ctpool_cond_destroy(ctpool_cond_t *cond);
int32_t ctpool_cond_init(ctpool_cond_t *cond);
void ctpool_cond_signal(ctpool_cond_t *cond);
void ctpool_cond_wait(ctpool_cond_t *cond, ctpool_mutex_t *mutex);

#endif
