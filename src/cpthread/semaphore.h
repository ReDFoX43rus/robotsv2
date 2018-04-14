/* POSIX Binary semaphore wrapper */

#ifndef PTHREAD_SEMAPHORE_H
#define PTHREAD_SEMAPHORE_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t sem_t;

int sem_init(sem_t *sem, int pshared, uint32_t value);
int sem_post(sem_t *sem);
int sem_try_take(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_destroy(sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: PTHREAD_SEMAPHORE_H */
