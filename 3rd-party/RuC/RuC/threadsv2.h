#ifndef RUC_THREADSV2_H
#define RUC_THREADSV2_H

#define MAX_THREADS 16
#define MAX_SEMS 16
#define MAX_MESSAGES_PER_THREAD 32

#ifndef configUSE_TASK_NOTIFICATIONS
#define configUSE_TASK_NOTIFICATIONS 1
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define DEFAULT_WAIT_TICKS pdMS_TO_TICKS(100)
#define STACK_SIZE 4096

typedef struct {
	int thread_n;
	int data;
} thmsg_t;

typedef enum {
	OBJ_FREE,
	OBJ_INITED
} objstatus_t;

typedef struct {
	TaskHandle_t handle;
	objstatus_t status;

	thmsg_t msgs[MAX_MESSAGES_PER_THREAD];
	int msgs_n;
} thread_t;

typedef struct {
	SemaphoreHandle_t semaphore;
	objstatus_t status;
} sema_t;

int t_init(void);
int t_destroy(void);

int t_create_inner(TaskFunction_t func, void* arg);
int t_getThNum(void);
int t_prepare_exit(void);
int t_exit(void);

int t_sem_create(int level);
int t_sem_wait(int numSem);
int t_sem_post(int numSem);

int t_msg_send(thmsg_t msg);
thmsg_t t_msg_receive(void);

#endif /* end of include guard: RUC_THREADSV2_H */
