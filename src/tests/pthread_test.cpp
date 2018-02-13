#include "pthread.h"
#include "semaphore.h"
#include "uart.h"

static sem_t outSem;

static void* task1(void *arg){
	for(int i = 0; i < 10; i++){
		sem_wait(&outSem);
		uart << "Iter #" << i << " from background thread" << endl;
		sem_post(&outSem);
	}

	return arg;
}

void pthread_test(void *arg){
	pthread_t thread;

	sem_init(&outSem, 1, 0);

	pthread_create(&thread, NULL, task1, NULL);

	for(int i = 0; i < 10; i++){
		sem_wait(&outSem);
		uart << "Iter #" << i << " from main thread" << endl;
		sem_post(&outSem);
	}

	pthread_join(thread, NULL);
	uart << "Thread joined!" << endl;

	sem_destroy(&outSem);
	uart << "Sem destroyed!" << endl;
}
