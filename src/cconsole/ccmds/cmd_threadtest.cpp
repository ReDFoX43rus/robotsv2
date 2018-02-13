#include "uart.h"
#include "pthread.h"
#include "semaphore.h"

static void* Thread(void *data){
	static int threadInc = 0;

	sem_wait(((sem_t*)data));
	uart << "Hello from thread " << threadInc++ << endl;
	sem_post(((sem_t*)data));
	return data;
}

extern void pthread_test(void*);
extern void rwlock_test(void*);

void CmdThreadtestHandler(int argc, char **argv){
	if(argc < 2){
		uart << "Usage: Threadtest num_threads" << endl;
		return;
	}

	sem_t sem;
	sem_init(&sem, 1, 0);

	int num = atoi(argv[1]);
	pthread_t *threads = new pthread_t[num];

	for(int i = 0; i < num; i++){
		pthread_create(&threads[i], NULL, Thread, (void*)&sem);
	}

	for(int i = 0; i < num; i++){
		pthread_join(threads[i], NULL);

		sem_wait(&sem);
		uart << "Thread " << i << " joined" << endl;
		sem_post(&sem);
	}

	sem_destroy(&sem);
	delete[] threads;

	pthread_test(NULL);
	//rwlock_test(NULL);
}
