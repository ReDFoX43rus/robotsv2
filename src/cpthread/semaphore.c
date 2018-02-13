#include "semaphore.h"
int sem_init(sem_t *sem, int pshared, uint32_t value){
	*sem = xSemaphoreCreateBinary();

	xSemaphoreGive(*sem);
	if(value)
		xSemaphoreTake(*sem, 0);

	return 0;
}

inline int sem_post(sem_t *sem){
	int val = xSemaphoreGive(*sem) == pdTRUE ? 0 : -1;
	return val;
}

inline int sem_try_take(sem_t *sem){
	return xSemaphoreTake(*sem, 10) == pdTRUE ? 0 : -1;
}

inline int sem_wait(sem_t *sem){
	while(sem_try_take(sem) < 0)
		;
	return 0;
}

inline int sem_destroy(sem_t *sem){
	vSemaphoreDelete(*sem);
	*sem = 0;
	return 0;
}
