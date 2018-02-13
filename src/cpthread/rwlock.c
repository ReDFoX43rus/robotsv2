#include "rwlock.h"

#define COUNT_RWLOCK 16

int pthread_rwlock_init(pthread_rwlock_t *rwlock, pthread_rwlockattr_t *attr){
	rwlock->readCount = 0;
	rwlock->writeCount = 0;

	int result = sem_init(&rwlock->rlock, 1, 0);
	if(result != 0)
		return -1;

	result = sem_init(&rwlock->wlock, 1, 0);
	if(result != 0)
		return -1;

	return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock){
	sem_wait(&rwlock->wlock);
	rwlock->writeCount++;

	return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock){
	sem_wait(&rwlock->rlock);

	rwlock->readCount++;
	/* Write operation is not available when sb is reading and virce versa */
	if(rwlock->readCount == 1)
		sem_wait(&rwlock->wlock);

	sem_post(&rwlock->rlock);

	return 0;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock){
	/* Case of writer realease */
	if(rwlock->writeCount > 0){
		rwlock->writeCount--;
		sem_post(&rwlock->wlock);
		return 0;
	}

	/* Case of one of readers release */
	sem_wait(&rwlock->rlock);

	if(rwlock->readCount > 0){
		rwlock->readCount--;
		if(rwlock->readCount == 0)
			sem_post(&rwlock->wlock); /* All readers done their works, writers can write now */
	}

	sem_post(&rwlock->rlock);

	return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock){
	sem_post(&rwlock->rlock);
	sem_post(&rwlock->wlock);
	sem_destroy(&rwlock->rlock);
	sem_destroy(&rwlock->wlock);

	return 0;
}
