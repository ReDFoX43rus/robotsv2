#include "threadsv2.h"

static thread_t threads[MAX_THREADS];
static SemaphoreHandle_t th_sem;

static sema_t sems[MAX_SEMS];
static SemaphoreHandle_t sem_sem;

#define TAKE_OR_DIE() if(xSemaphoreTake(th_sem, DEFAULT_WAIT_TICKS) != pdTRUE) \
 						return -1
#define GIVE_OR_DIE() if(xSemaphoreGive(th_sem) != pdTRUE) \
						return -10

#define STAKE_OR_DIE() if(xSemaphoreTake(sem_sem, DEFAULT_WAIT_TICKS) != pdTRUE) return -1
#define SGIVE_OR_DIE() if(xSemaphoreGive(sem_sem) != pdTRUE) return -10

typedef struct {
	TaskFunction_t func;
	void *arg;
} looper_arg_t;

static void looper(void *arg){
	void *data = ((looper_arg_t*)arg)->arg;
	TaskFunction_t func = ((looper_arg_t*)arg)->func;

	free(arg);

	while(1)
		func(data);
}

/* SHOULD BE CALLED ONLY WITH TAKEN SEMAPHORE */
static inline int get_free_th_num(void){
	for(int i = 0; i < MAX_THREADS; i++){
		if(threads[i].status == OBJ_FREE)
			return i;
	}

	return -1;
}

/* SHOULD BE CALLED ONLY WITH TAKEN SEMAPHORE */
static inline int get_free_sem_num(void){
	for(int i = 0; i < MAX_SEMS; i++){
		if(sems[i].status == OBJ_FREE)
			return i;
	}

	return -1;
}

int t_init(void){
	th_sem = xSemaphoreCreateMutex();
	if(th_sem == NULL)
		return -2;

	TAKE_OR_DIE();
	threads[0].handle = xTaskGetCurrentTaskHandle();
	threads[0].status = OBJ_INITED;
	threads[0].msg_sem = xSemaphoreCreateMutex();
	GIVE_OR_DIE();

	sem_sem = xSemaphoreCreateMutex();
	if(!sem_sem)
		return -3;

	return 0;
}
int t_destroy(void){
	TAKE_OR_DIE();
	/* Start from 1, don't remove main task */
	for(int i = 1; i < MAX_THREADS; i++){
		if(threads[i].status == OBJ_INITED){
			vSemaphoreDelete(threads[i].msg_sem);
			vTaskDelete(threads[i].handle);
		}
	}
	GIVE_OR_DIE();

	STAKE_OR_DIE();
	for(int i = 0; i < MAX_SEMS; i++){
		if(sems[i].status == OBJ_INITED){
			xSemaphoreGive(sems[i].semaphore);
			sems[i].status = OBJ_FREE;
			vSemaphoreDelete(sems[i].semaphore);
		}
	}
	SGIVE_OR_DIE();

	vSemaphoreDelete(th_sem);
	vSemaphoreDelete(sem_sem);

	return 0;
}
int t_create_inner(TaskFunction_t func, void* arg){
	TAKE_OR_DIE();

	int new_thread_id = get_free_th_num();
	if(new_thread_id == -1){
		GIVE_OR_DIE();
		return -1;
	}

	looper_arg_t *args = (looper_arg_t*)malloc(sizeof(looper_arg_t));
	args->func = func;
	args->arg = arg;

	if(xTaskCreate(looper, "ruc_task", STACK_SIZE, args, 1, &(threads[new_thread_id].handle)) != pdPASS){
		GIVE_OR_DIE();
		return -1;
	}

	threads[new_thread_id].status = OBJ_INITED;
	threads[new_thread_id].msg_sem = xSemaphoreCreateMutex();
	threads[new_thread_id].msgs_n = 0;

	GIVE_OR_DIE();
	return new_thread_id;
}
int t_getThNum(void){
	int retval = -2;
	TAKE_OR_DIE();

	TaskHandle_t current = xTaskGetCurrentTaskHandle();
	for(int i = 0; i < MAX_THREADS; i++){
		if(threads[i].handle == current && threads[i].status == OBJ_INITED){
			retval = i;
			break;
		}
	}

	GIVE_OR_DIE();
	return retval;
}
int t_exit(int num){
	/* we cannot exit from 0 (main) thread */
	if(num <= 0 || num >= MAX_THREADS)
		return -2;

	TAKE_OR_DIE();

	if(threads[num].status != OBJ_INITED){
		GIVE_OR_DIE();
		return -3;
	}

	/* remove semaphore and message */
	threads[num].status = OBJ_FREE;
	vTaskDelete(threads[num].handle);
	threads[num].handle = 0;
	vSemaphoreDelete(threads[num].msg_sem);

	GIVE_OR_DIE();

	return 0;
}

int t_sem_create(int level){
	STAKE_OR_DIE();
	int new_id = get_free_sem_num();
	if(new_id == -1){
		SGIVE_OR_DIE();
		return -2;
	}

	sems[new_id].semaphore = xSemaphoreCreateMutex();
	if(!sems[new_id].semaphore){
		SGIVE_OR_DIE();
		return -3;
	}

	sems[new_id].status = OBJ_INITED;

	SGIVE_OR_DIE();

	if(level && t_sem_wait(new_id) != 0){
		t_sem_destroy(new_id);
		return -4;
	}

	return new_id;
}
int t_sem_destroy(int sem){
	if(sem < 0 || sem >= MAX_SEMS)
		return -2;

	STAKE_OR_DIE();

	if(sems[sem].status != OBJ_INITED){
		SGIVE_OR_DIE();
		return -3;
	}

	vSemaphoreDelete(sems[sem].semaphore);
	sems[sem].status = OBJ_FREE;

	SGIVE_OR_DIE();
	return 0;
}
int t_sem_wait(int numSem){
	STAKE_OR_DIE();
	if(xSemaphoreTake(sems[numSem].semaphore, DEFAULT_WAIT_TICKS) != pdTRUE)
		return -2;
	SGIVE_OR_DIE();
	return 0;
}
int t_sem_post(int numSem){
	STAKE_OR_DIE();
	if(xSemaphoreGive(sems[numSem].semaphore) != pdTRUE)
		return -2;
	SGIVE_OR_DIE();
	return 0;
}

int t_msg_send(thmsg_t msg){
	int current_thread_n = t_getThNum();
	if(current_thread_n < 0)
		return -6;

	TAKE_OR_DIE();
	int n = msg.thread_n;
	if(threads[n].status != OBJ_INITED){
		GIVE_OR_DIE();
		return -2;
	}
	thread_t *to = &threads[n];
	TaskHandle_t to_handle = to->handle;
	GIVE_OR_DIE();

	int resume_receiver = 0;
	while(1){

		if(xSemaphoreTake(to->msg_sem, DEFAULT_WAIT_TICKS) != pdTRUE)
			return -3;

		if(to->msgs_n <= 0){
			resume_receiver = 1;
		}

		if(to->msgs_n >= MAX_SEMS){
			xSemaphoreGive(to->msg_sem);
			vTaskSuspend(NULL);
			continue;
		}

		to->msgs[to->msgs_n] = msg;
		to->msgs[to->msgs_n++].thread_n = current_thread_n;

		if(resume_receiver){
			resume_receiver = 0;
			while(eTaskGetState(to_handle) != eSuspended)
				;//vTaskDelay(pdMS_TO_TICKS(50));
			vTaskResume(to_handle);
		}

		if(xSemaphoreGive(to->msg_sem) != pdTRUE)
			return -4;

		return 0;

	}
	return 0;
}
thmsg_t t_msg_receive(void){
	thmsg_t retval = {
		.thread_n = 0,
		.data = 0
	};

	int current_thread_n = t_getThNum();
	if(current_thread_n < 0){
		retval.thread_n = -2;
		return retval;
	}

	if(xSemaphoreTake(th_sem, DEFAULT_WAIT_TICKS) != pdTRUE){
		retval.thread_n = -1;
		return retval;
	}
	thread_t *current = &threads[current_thread_n];
	xSemaphoreGive(th_sem);

	int resume_sender = 0;
	while(1){
		if(xSemaphoreTake(current->msg_sem, DEFAULT_WAIT_TICKS) != pdTRUE){
			retval.thread_n = -4;
			return retval;
		}

		if(current->msgs_n >= MAX_SEMS){
			resume_sender = 1;
		}

		if(current->msgs_n <= 0){
			xSemaphoreGive(current->msg_sem);
			vTaskSuspend(NULL);
			continue;
		}

		retval = current->msgs[--current->msgs_n];

		int from_id = retval.thread_n;
		while(xSemaphoreTake(th_sem, DEFAULT_WAIT_TICKS) != pdTRUE)
			;

		TaskHandle_t from_handle = threads[from_id].handle;

		xSemaphoreGive(th_sem);

		if(resume_sender){
			resume_sender = 0;
			while(eTaskGetState(from_handle) != eSuspended)
				;//vTaskDelay(pdMS_TO_TICKS(50));
			vTaskResume(from_handle);
		}

		xSemaphoreGive(current->msg_sem);

		break;
	}

	return retval;
}

#define RUC_TEST_MAX_WORKERS 4

static int ruc_sem = -1;

void ruc_test_worker(void *arg){
	t_sem_wait(ruc_sem);
	printf("Hello from thread %d\n", (int)arg);
	t_sem_post(ruc_sem);

	thmsg_t msg = {
		.thread_n = RUC_TEST_MAX_WORKERS,
		.data = 10 + (int)arg
	};

	int res = t_msg_send(msg);
	printf("#%d: t_msg_send: %d\n", (int)arg, res);

	// vTaskDelay(pdMS_TO_TICKS(500));
}

void ruc_test_msg_reveice(void *arg){
	thmsg_t msg;
	msg = t_msg_receive();
	printf("From: %d, data: %d\n", msg.thread_n, msg.data);
}

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
