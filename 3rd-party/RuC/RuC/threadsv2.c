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

static TaskHandle_t collectorHandle;
/* stack of threads discriptors to release */
static SemaphoreHandle_t clctr_sem = 0;
/* stack pointer */
static int release_p = -1;
static int threads_to_release[MAX_THREADS];

static inline int is_collector_suspended(void){
	return eTaskGetState(collectorHandle) == eSuspended;
}

static void collector(void* arg){
	int th_id;
	while(1){
		printf("collector is trying to take sem 0x%X\n", (uint32_t)clctr_sem);
		vTaskDelay(pdMS_TO_TICKS(1000));
		if(xSemaphoreTake(clctr_sem, DEFAULT_WAIT_TICKS) != pdTRUE){
			printf("collector failed to take sem\n");
			vTaskDelay(DEFAULT_WAIT_TICKS);
			continue;
		} else printf("collector took sem\n");

		if(release_p < 0){
			xSemaphoreGive(clctr_sem);
			printf("collector released sem 1, suspending...\n");
			vTaskSuspend(NULL);
			printf("collector resumed\n");
			vTaskDelay(pdMS_TO_TICKS(1000));
			continue;
		}

		th_id = threads_to_release[release_p--];
		xSemaphoreGive(clctr_sem);
		printf("collector released sem 2\n");

		while(xSemaphoreTake(th_sem, DEFAULT_WAIT_TICKS) != pdTRUE)
			;

		/* disable interrupts */
		taskDISABLE_INTERRUPTS();
		/* disable scheduler */
		vTaskSuspendAll();

		// threads[th_id].status = OBJ_FREE;
		// vTaskDelete(threads[th_id].handle);
		// threads[th_id].handle = 0;
		// vSemaphoreDelete(threads[th_id].msg_sem);

		/* enable interrupts */
		taskENABLE_INTERRUPTS();
		/* enable scheduler */
		xTaskResumeAll();

		xSemaphoreGive(th_sem);
	}
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
	int debug = 0;

	// printf("init %d\n", debug++);

	th_sem = xSemaphoreCreateMutex();
	if(th_sem == NULL)
		return -2;

	// printf("init %d\n", debug++);

	TAKE_OR_DIE();
	// printf("init %d\n", debug++);
	threads[0].handle = xTaskGetCurrentTaskHandle();
	threads[0].status = OBJ_INITED;
	threads[0].msg_sem = xSemaphoreCreateMutex();
	GIVE_OR_DIE();

	// printf("init %d\n", debug++);

	sem_sem = xSemaphoreCreateMutex();
	if(!sem_sem)
		return -3;

	// printf("init %d\n", debug++);

	release_p = -1;
	clctr_sem = xSemaphoreCreateMutex();
	if(!clctr_sem)
		return -4;

	// printf("init %d\n", debug++);

	// if(xTaskCreate(collector, "ruc_collector", STACK_SIZE, NULL, 5, &collectorHandle) != pdTRUE)
	// 	return -5;

	// printf("init %d\n", debug++);

	return 0;
}
int t_destroy(void){
	int debug = 0;

	// printf("destroy %d\n", debug++);

	TAKE_OR_DIE();
	// printf("destroy %d\n", debug++);
	for(int i = 0; i < MAX_THREADS; i++){
		if(threads[i].status == OBJ_INITED){
			vSemaphoreDelete(threads[i].msg_sem);
			vTaskDelete(threads[i].handle);
		}
	}
	GIVE_OR_DIE();

	// printf("destroy %d\n", debug++);

	STAKE_OR_DIE();
	// printf("destroy %d\n", debug++);
	for(int i = 0; i < MAX_SEMS; i++){
		if(sems[i].status == OBJ_INITED){
			xSemaphoreGive(sems[i].semaphore);
			sems[i].status = OBJ_FREE;
			vSemaphoreDelete(sems[i].semaphore);
		}
	}
	SGIVE_OR_DIE();

	// printf("destroy %d\n", debug++);

	vSemaphoreDelete(th_sem);
	vSemaphoreDelete(sem_sem);

	vTaskDelete(collectorHandle);
	vSemaphoreDelete(clctr_sem);

	return 0;
}
int t_create_inner(TaskFunction_t func, void* arg){
	int debug = 0;
	// printf("create_inner %d\n", debug++);
	TAKE_OR_DIE();

	// printf("create_inner %d\n", debug++);

	int new_thread_id = get_free_th_num();
	if(new_thread_id == -1){
		GIVE_OR_DIE();
		return -1;
	}

	// printf("create_inner %d\n", debug++);

	if(xTaskCreate(func, "ruc_task", STACK_SIZE, arg, 1, &(threads[new_thread_id].handle)) != pdPASS){
		GIVE_OR_DIE();
		return -1;
	}

	// printf("create_inner %d\n", debug++);

	threads[new_thread_id].status = OBJ_INITED;
	threads[new_thread_id].msg_sem = xSemaphoreCreateMutex();
	threads[new_thread_id].msgs_n = 0;

	// printf("create_inner %d\n", debug++);

	GIVE_OR_DIE();
	// printf("create_inner %d\n", debug++);
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
int t_exit(void){
	int debug = 0;
	// printf("exit %d\n", debug++);
	int num = t_getThNum();
	if(num < 0)
		return num*100;

	printf("exit %d, taking sem 0x%X\n", debug++, (uint32_t)clctr_sem);

	while(xSemaphoreTake(clctr_sem, DEFAULT_WAIT_TICKS) != pdTRUE)
		vTaskDelay(DEFAULT_WAIT_TICKS);

	// printf("exit %d, sem: 0x%X\n", debug++, (uint32_t)clctr_sem);

	threads_to_release[release_p++] = num;

	while(is_collector_suspended() != 1)
		vTaskDelay(pdMS_TO_TICKS(5));

	// printf("exit %d, sem: 0x%X\n", debug++, (uint32_t)clctr_sem);

	vTaskResume(collectorHandle);

	// printf("exit: collector resumed, giving sem: 0x%X\n", (uint32_t)clctr_sem);

	if(xSemaphoreGive(clctr_sem) != pdTRUE)
		printf("exit cannot give sem\n");
	else printf("exit gave sem 0x%X\n", (uint32_t)clctr_sem);

	// printf("exit %d\n", debug++);

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
				vTaskDelay(pdMS_TO_TICKS(50));
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

		retval = current->msgs[current->msgs_n];
		current->msgs_n--;

		int from_id = retval.thread_n;
		while(xSemaphoreTake(th_sem, DEFAULT_WAIT_TICKS) != pdTRUE)
			;

		TaskHandle_t from_handle = threads[from_id].handle;

		xSemaphoreGive(th_sem);

		if(resume_sender){
			resume_sender = 0;
			while(eTaskGetState(from_handle) != eSuspended)
				vTaskDelay(pdMS_TO_TICKS(50));
			vTaskResume(from_handle);
		}

		xSemaphoreGive(current->msg_sem);

		break;
	}

	return retval;
}

#define RUC_TEST_MAX_WORKERS 2

static int counter = 1;
static int sem;
static int max_workers = RUC_TEST_MAX_WORKERS;
static SemaphoreHandle_t test_sem;

static void release_test(int from){
	xSemaphoreTake(test_sem, DEFAULT_WAIT_TICKS);

	printf("%d workers left\n", --max_workers);

	xSemaphoreGive(test_sem);

	if(!max_workers){
		//t_destroy();
		//vSemaphoreDelete(test_sem);
	}// else t_exit();
}

void ruc_test_worker(void *arg){
	// int th_num = (int)arg;
	// int err;
	// for(int i = 0; i < 5; i++){
	// 	if((err = t_sem_wait(sem)) < 0){
	// 		printf("#%d: cannot take sem %d\n", th_num, err);
	// 		release_test(th_num);
	// 		return;
	// 	}
	// 	// printf("#%d: counter: %d\n", th_num, counter++);
	// 	vTaskDelay(pdMS_TO_TICKS(1200));
	// 	if((err = t_sem_post(sem)) < 0){
	// 		printf("#%d: cannot release sem %d\n", th_num, err);
	// 		release_test(th_num);
	// 		return;
	// 	}
	// }
    //
	// release_test(th_num);
	vTaskDelay(pdMS_TO_TICKS(100));
}

void test_ruc_threadsv2(void){
	//test_sem = xSemaphoreCreateMutex();

	//printf("init: %d\n", t_init());
	//int w = t_create_inner(ruc_test_worker, NULL);
	int w = xTaskCreate(ruc_test_worker, "ruc_test_worker", 2048, NULL, 1, NULL) == pdTRUE;
	printf("W: %d\n", w);
	// sem = t_sem_create(0);
	// int workers[RUC_TEST_MAX_WORKERS];
	// for(int i = 0; i < RUC_TEST_MAX_WORKERS; i++){
	// 	workers[i] = t_create_inner(worker, (void*)(i+1));
	// 	if(!workers[i]){
	// 		printf("Cannot create thread %d %d\n", i, workers[i]);
	// 	}
	// }
}
