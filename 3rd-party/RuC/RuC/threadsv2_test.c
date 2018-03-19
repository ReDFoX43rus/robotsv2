#include "unistd.h"
#include "threadsv2.h"

#define RUC_TEST_MAX_WORKERS 4

static int ruc_sem = -1;

void* ruc_test_worker(void *arg){
	t_sem_wait(ruc_sem);
	printf("Hello from thread %d\n", (int)arg);
	t_sem_post(ruc_sem);

	thmsg_t msg = {
		.thread_n = RUC_TEST_MAX_WORKERS,
		.data = 10 + (int)arg
	};

	int res = t_msg_send(msg);
	printf("#%d: t_msg_send: %d\n", (int)arg, res);

	return NULL;
}

void* ruc_test_msg_reveice(void *arg){
	thmsg_t msg;
	msg = t_msg_receive();
	printf("From: %d, data: %d\n", msg.thread_n, msg.data);
	return NULL;
}

/* Features:
 * t_init t_destroy
 * thread creation and destruction
 * semaphore creation and destruction
 * semaphore wait and post
 * messages */
void test_ruc_threadsv2(void){
	int workers[RUC_TEST_MAX_WORKERS];

	t_init();

	ruc_sem = t_sem_create(0);

	printf("Creating threads...\n");
	for(int i = 0; i < RUC_TEST_MAX_WORKERS-1; i++)
		workers[i] = t_create_inner(ruc_test_worker, (void*)(i+1));

	workers[RUC_TEST_MAX_WORKERS-1] = t_create_inner(ruc_test_msg_reveice, NULL);
	printf("Receiver descriptor: %d\n", workers[RUC_TEST_MAX_WORKERS-1]);

	vTaskDelay(pdMS_TO_TICKS(10000));
	printf("Removing threads...\n");

	for(int i = 0; i < RUC_TEST_MAX_WORKERS; i++)
		t_exit(workers[i]);

	printf("All threads removed\n");

	t_sem_destroy(ruc_sem);
	t_destroy();
	printf("OK\n");
}

static void *hello(void*arg){
	printf("Hello from thread\n");
	sleep(1);
	return NULL;
}

void test_ruc_hello(void){
	t_init();
	t_create_inner(hello, NULL);
	sleep(5);
	printf("Calling t_destroy\n");
	t_destroy();
}
