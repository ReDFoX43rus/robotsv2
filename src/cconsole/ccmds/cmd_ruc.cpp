#include "uart.h"
#include "pthread.h"
#include "cpthread/dsem.h"

#include "freertos/task.h"

extern "C" void *ruc_import(void *filename);

static pthread_cond_t cv;
static pthread_mutex_t lock;

static void *thread(void *v) {
	printf("Locking and waiting. Type unlock to unlock me.\n");
	pthread_mutex_lock(&lock);
	printf("Mutex locked\n");
	pthread_cond_wait(&cv, &lock);
	printf("I've been unlocked.\n");
	pthread_mutex_unlock(&lock);
	return NULL;
}

void CmdRucHandler(int argc, char **argv){
	/*pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cv, NULL);

	pthread_t th;
	pthread_create(&th, NULL, thread, NULL);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	uart << "Im here" << endl;
	pthread_cond_signal(&cv);
	uart << "Signal sent" << endl;

	pthread_join(th, NULL);
	uart << "Thread joined" << endl;

	pthread_cond_destroy(&cv);
	pthread_mutex_destroy(&lock);

	return;*/

	if(argc < 2){
		uart << "Usage: ruc filename" << endl;
		return;
	}

	ruc_import(argv[1]);

	/*if(pthread_join(ruc_thread, NULL)) {
		uart << "Error joining thread" << endl;
		return;
	}

	uart << "RuC finished" << endl;*/
}
