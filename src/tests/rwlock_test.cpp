#include "pthread.h"
#include "semaphore.h"
#include "cpthread/rwlock.h"
#include "uart.h"

static uint8_t data = 0;
static pthread_rwlock_t rwlock;
static sem_t uartSem;

static void *reader(void *arg){
	int readerNum = *(int*)arg;

	while(1){
		pthread_rwlock_rdlock(&rwlock);
		if(data >= 4){
			pthread_rwlock_unlock(&rwlock);
			return arg;
		}

		sem_wait(&uartSem);
		uart << "Reader #" <<readerNum << " data: " << data << endl;
		sem_post(&uartSem);

		pthread_rwlock_unlock(&rwlock);

		usleep(readerNum*700*10); // magic number
	}

	return arg;
}

void rwlock_test(void *arg){
	sem_init(&uartSem, 1, 0);
	pthread_rwlock_init(&rwlock, NULL);

	pthread_t readers[8];
	int args[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	for(int i = 0; i < 8; i++){
		pthread_create(&readers[i], NULL, reader, (void*)&args[i]);
	}

	for(int i = 0; i < 4; i++){
		pthread_rwlock_wrlock(&rwlock);
		data = i + 1;;
		pthread_rwlock_unlock(&rwlock);
		sem_wait(&uartSem);
		uart << "Writer updated data value to " << i+1 << endl;
		sem_post(&uartSem);
	}

	for(int i = 0; i < 8; i++)
		pthread_join(readers[i], NULL);

	pthread_rwlock_destroy(&rwlock);
	sem_destroy(&uartSem);
}
