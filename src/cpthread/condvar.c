#include "condvar.h"
#include "sys/errno.h"
#include "stdio.h"

inline static pthread_cond *get_cond(pthread_cond_t *cond){
	return (pthread_cond*)(*cond);
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr){
	pthread_cond *c = (pthread_cond*)malloc(sizeof(pthread_cond));

	if(!c)
		return ENOMEM;

	c->waiters = 0;
	c->s = dsem_init(0, 999999);
	c->h = dsem_init(0, 999999);

	sem_init(&c->waiters_sem, 1, 0);

	*cond = (pthread_cond_t)c;

	return 0;
}
int pthread_cond_destroy(pthread_cond_t *cond){
	pthread_cond *c = get_cond(cond);

	dsem_destroy(c->s);
	dsem_destroy(c->h);

	sem_destroy(&c->waiters_sem);

	free(c);

	return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
	pthread_cond *c = get_cond(cond);

	sem_wait(&c->waiters_sem);
	c->waiters++;
	sem_post(&c->waiters_sem);

	printf("Waiters incremented\n");

	pthread_mutex_unlock(mutex);

	printf("Mutex nunlocked\n");

	dsem_p(c->s);
	printf("c->s P\n");
	dsem_v(c->h);
	printf("c->h V\n");

	pthread_mutex_lock(mutex);

	printf("Mutex locked\n");

	return 0;
}
int pthread_cond_signal(pthread_cond_t *cond){
	pthread_cond *c = get_cond(cond);

	sem_wait(&c->waiters_sem);

	printf("Waiters: %d\n", c->waiters);

	if(c->waiters > 0){
		c->waiters--;

		dsem_v(c->s);
		printf("c->s V\n");
		dsem_p(c->h);
		printf("c->h P\n");
	}
	sem_post(&c->waiters_sem);

	return 0;
}
