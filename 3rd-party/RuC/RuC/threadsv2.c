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
	GIVE_OR_DIE();

	sem_sem = xSemaphoreCreateMutex();
	if(!sem_sem)
		return -3;

	return 0;
}
int t_destroy(void){
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

	if(xTaskCreatePinnedToCore(func, "ruc_task", STACK_SIZE, arg, 1, &threads[new_thread_id].handle, 0) != pdPASS){
		GIVE_OR_DIE();
		return -1;
	}

	threads[new_thread_id].status = OBJ_INITED;

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

int t_exit(void){
	int num = t_getThNum();

	TAKE_OR_DIE();
	threads[num].status = OBJ_FREE;
	GIVE_OR_DIE();

	vTaskDelete(NULL);
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

	if(level && t_sem_wait(new_id) != 0)
		return -4;

	return new_id;
}
int t_sem_wait(int numSem){
	STAKE_OR_DIE();
	while(xSemaphoreTake(sems[numSem].semaphore, DEFAULT_WAIT_TICKS) != pdTRUE)
		;
	SGIVE_OR_DIE();
	return 0;
}
int t_sem_post(int numSem){
	STAKE_OR_DIE();
	while(xSemaphoreGive(sems[numSem].semaphore) != pdTRUE)
		;
	SGIVE_OR_DIE();
	return 0;
}

int t_msg_send(thmsg_t msg){
	return 0;
}
thmsg_t t_msg_receive(void){
	thmsg_t test;
	return test;
}
