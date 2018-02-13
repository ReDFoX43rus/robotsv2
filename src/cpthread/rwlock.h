#ifndef CPTHREAD_RWLOCK_H
#define CPTHREAD_RWLOCK_H

#include "semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t pthread_rwlockattr_t;
typedef struct pthread_rwlock{
	uint32_t readCount;
	uint32_t writeCount;
	sem_t rlock;
	sem_t wlock;
} pthread_rwlock_t;

int pthread_rwlock_init(pthread_rwlock_t *rwlock, pthread_rwlockattr_t *attr);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: CPTHREAD_RWLOCK_H */
