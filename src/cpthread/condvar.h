#ifndef CPTHREAD_CONDVAR_H
#define CPTHREAD_CONDVAR_H

/* To understand this please check out: https://www.microsoft.com/en-us/research/wp-content/uploads/2004/12/ImplementingCVs.pdf */

#include "dsem.h"
#include "semaphore.h"
#include "pthread.h" // declaration of pthread functions

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _pthread_cond{
	dsem_t *s;
	dsem_t *h;
	sem_t waiters_sem;
	int waiters;
} pthread_cond;

// int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
// int pthread_cond_destroy(pthread_cond_t *cond);
// int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
// int pthread_cond_signal(pthread_cond_t *cond);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: CPTHREAD_CONDVAR_H */
