//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "th_static.h"

#include "cpthread/condvar.h"
#include "cpthread/rwlock.h"

#define STACK_SIZE 4096

#define TRUE 1
#define FALSE 0

#ifndef _REENTRANT
#define _REENTRANT
#endif

struct __threadInfo
{
	//pthread_t th;
	TaskHandle_t handle;
	int isDetach;

	pthread_cond_t cond;
	pthread_mutex_t lock;
	struct msg_info msgs[__COUNT_MSGS_FOR_TH];
	int countMsg;

	struct __threadInfo *parent;
};

static int __countTh = 1;
static struct __threadInfo __threads[__COUNT_TH];

static int __countSem = 0;
// sem_t __sems[__COUNT_SEM];
static sem_t __sems[__COUNT_SEM];
static pthread_rwlock_t __lock_t_create;
static pthread_rwlock_t __lock_t_sem_create;

static int inited = 0;
static sem_t utilsem;
//void perror(const char *str);

void t_init()
{
	int res = pthread_rwlock_init(&__lock_t_create, NULL);
	if (res != 0)
	{
		perror("t_init : pthread_rwlock_init of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}
	//__threads[0].th = pthread_self();
	__threads[0].handle = xTaskGetCurrentTaskHandle();
	__threads[0].isDetach = TRUE;
	__threads[0].parent = NULL;

	res = pthread_cond_init(&(__threads[0].cond), NULL);
	if (res != 0)
	{
		perror("t_init : pthread_cond_init of __threads[0].cond failed");
		exit(EXIT_FAILURE);
	}

	res = pthread_mutex_init(&(__threads[0].lock), NULL);
	if (res != 0)
	{
		perror("t_init : pthread_mutex_init of __threads[0].lock failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_rwlock_init(&__lock_t_sem_create, NULL);
	if (res != 0)
	{
		perror("t_init : pthread_rwlock_init of __lock_t_sem_create failed");
		exit(EXIT_FAILURE);
	}

	if(!inited)
		sem_init(&utilsem, 1, 0);

	printf("t_init done\n");
	inited = 1;
}

int __t_create(void (*func)(void *), void *arg, int isDetach)
{
	int current_th_num = t_getThNum();
	struct __threadInfo* currentThread = NULL;

	if(pthread_rwlock_rdlock(&__lock_t_create)){
		perror("__t_create : pthread_rwlock_rdlock create failed");
		exit(EXIT_FAILURE);
	}

	sem_wait(&utilsem);
	printf("current th num: %d\n", current_th_num);
	sem_post(&utilsem);

	currentThread = &__threads[current_th_num];

	if(pthread_rwlock_unlock(&__lock_t_create)){
		perror("__t_create : pthread_rwlock_unlock failed");
		exit(EXIT_FAILURE);
	}

	int retVal;
	int res = pthread_rwlock_wrlock(&__lock_t_create);
	if (res != 0)
	{
		perror("__t_create : pthread_rwlock_wrlock of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}

	//res = pthread_create(&th, attr, func, arg);

	TaskHandle_t xHandle;
	BaseType_t result = xTaskCreatePinnedToCore(func, "dbg_task", STACK_SIZE, arg, 1, &xHandle, 0);
	if (result != pdPASS)
	{
		perror("t_create : Thread creation failed");
		exit(EXIT_FAILURE);
	}

	/*if (attr)
	{
		res = pthread_attr_destroy(attr);
		if(res != 0)
		{
			perror("t_create : Thread attribute destroy failed");
			exit(EXIT_FAILURE);
		}
	}*/
	if (__countTh >= __COUNT_TH)
	{
		perror("t_create : Trying to create too much threads");
		exit(EXIT_FAILURE);
	}

	__threads[__countTh].handle = xHandle;
	__threads[__countTh].isDetach = isDetach;
	__threads[__countTh].parent = isDetach ? NULL : currentThread;

	res = pthread_cond_init(&(__threads[__countTh].cond), NULL);
	if (res != 0)
	{
		perror("__t_create : pthread_cond_init of __threads[__countTh].cond failed");
		exit(EXIT_FAILURE);
	}

	res = pthread_mutex_init(&(__threads[__countTh].lock), NULL);
	if (res != 0)
	{
		perror("__t_create : pthread_mutex_init of __threads[__countTh].lock failed");
		exit(EXIT_FAILURE);
	}

	__threads[__countTh].countMsg = 0;
	retVal = __countTh++;

	res = pthread_rwlock_unlock(&__lock_t_create);
	if (res != 0)
	{
		perror("__t_create : pthread_rwlock_unlock of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}

	sem_wait(&utilsem);
	printf("Thread created\n");
	sem_post(&utilsem);
	return retVal;
}

int t_create_inner(void (*func)(void *), void *arg)
{
	return __t_create(func, arg, FALSE);
}

/*int t_createDetached(void* (*func)(void *))
{
	pthread_attr_t attr;

	int res = pthread_attr_init(&attr);
	if (res != 0)
	{
		perror("t_createDetached : Attribute creation failed");
		exit(EXIT_FAILURE);
	}

	res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (res != 0)
	{
		perror("t_createDetached : Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}

	return __t_create(&attr, func, NULL, TRUE);
}*/

void t_exit()
{
	vTaskDelete(NULL);
}

void t_notify_finished(void){
	sem_wait(&utilsem);
	printf("Trying to notify...\n");
	sem_post(&utilsem);

	int current_th_num = t_getThNum();

	if(pthread_rwlock_rdlock(&__lock_t_create)){
		perror("t_notify_finished : pthread_rwlock_rdlock failed");
		exit(EXIT_FAILURE);
	}

	if(__threads[0].handle == xTaskGetCurrentTaskHandle()){
		printf("Nothing to notify. This is the main thread\n");
		return;
	}

	struct __threadInfo* parent = __threads[current_th_num].parent;

	if(!parent){
		if(pthread_rwlock_unlock(&__lock_t_create)){
			perror("t_notify_finished : pthread_rwlock_unlock 1 failed");
			exit(EXIT_FAILURE);
		}
		printf("no parent - nothing to notify\n");
		return;
	}

	if(pthread_rwlock_unlock(&__lock_t_create)){
		perror("t_notify_finished : pthread_rwlock_unlock 2 failed");
		exit(EXIT_FAILURE);
	}

	// set to parent's notification value bit - id of current thread
	xTaskNotify(parent->handle, 1 << current_th_num, eSetBits);
}

void t_join(int numTh)
{
	sem_wait(&utilsem);
	printf("Joining thread: %d\n", numTh);
	sem_post(&utilsem);

	int res = pthread_rwlock_rdlock(&__lock_t_create);
	if (res != 0)
	{
		perror("t_join : pthread_rwlock_rdlock of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}

	if (numTh > 0 && numTh < __countTh)
	{
		if (!__threads[numTh].isDetach)
		{
			res = pthread_rwlock_unlock(&__lock_t_create);
			if (res != 0)
			{
				perror("t_join : pthread_rwlock_unlock of __lock_t_create failed");
				exit(EXIT_FAILURE);
			}

			sem_wait(&utilsem);
			printf("Trying to wait notification...\n");
			sem_post(&utilsem);

			uint32_t notify_value;
			xTaskNotifyWait(0, ULONG_MAX, &notify_value, pdMS_TO_TICKS(100));

			sem_wait(&utilsem);
			printf("Notification handled\n");
			sem_post(&utilsem);
		}
		else
		{
			perror("t_join : Thread join failed - thread is detached");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("t_join : Thread join failed - number of thread is out of range");
		exit(EXIT_FAILURE);
	}

	sem_wait(&utilsem);
	printf("thread joined!\n");
	sem_post(&utilsem);
}

int t_getThNum()
{
	TaskHandle_t current_handle = xTaskGetCurrentTaskHandle();
	int index, i;
	int res = pthread_rwlock_rdlock(&__lock_t_create);
	if (res != 0)
	{
		perror("t_getThNum : pthread_rwlock_rdlock of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}

	index = -1;

	for (i = 0; i < __countTh; i++)
	{
		if (current_handle == __threads[i].handle)
		{
			index = i;
			break;
		}
	}

	if (index != -1)
	{
		res = pthread_rwlock_unlock(&__lock_t_create);
		if (res != 0)
		{
			perror("t_getThNum : pthread_rwlock_unlock of __lock_t_create failed");
			exit(EXIT_FAILURE);
		}

		return index;
	}
	perror("t_getThNum : Thread is not registered");
	exit(EXIT_FAILURE);
}

void t_sleep(int miliseconds)
{
	//sleep(seconds * 1000);
	//usleep(miliseconds * 1000);
	vTaskDelay(miliseconds / portTICK_PERIOD_MS);
}

int t_sem_create(int level)
{
	if (__countSem >= __COUNT_SEM)
	{
		perror("t_create : Trying to create too much semaphores");
		exit(EXIT_FAILURE);
	}

	int res = pthread_rwlock_wrlock(&__lock_t_sem_create), retVal;
	//char csem[10];
	if (res != 0)
	{
		perror("t_sem_create : pthread_rwlock_wrlock of __lock_t_sem_create failed");
		exit(EXIT_FAILURE);
	}
	//sprintf(csem, "%d", __countSem);
	//sem_unlink(csem);
	//sem = sem_open(csem, O_CREAT, S_IRUSR | S_IWUSR, level);

	res = sem_init(&__sems[__countSem], 1, level);
	if (res)
	{
		perror("t_sem_create : Semaphore initilization failed");
		exit(EXIT_FAILURE);
	}

	retVal = __countSem++;

	res = pthread_rwlock_unlock(&__lock_t_sem_create);
	if (res != 0)
	{
		perror("t_sem_create : pthread_rwlock_unlock of __lock_t_sem_create failed");
		exit(EXIT_FAILURE);
	}
	return retVal;
}

void t_sem_wait(int numSem)
{
	int res = pthread_rwlock_rdlock(&__lock_t_sem_create);
//    printf("t_sem_wait numSem= %i __countSem=  %i\n", numSem, __countSem);
	if (res != 0)
	{
		perror("t_sem_wait : pthread_rwlock_rdlock of __lock_t_sem_create failed");
		exit(EXIT_FAILURE);
	}
	if (numSem >= 0 && numSem < __countSem)
	{
		res = pthread_rwlock_unlock(&__lock_t_sem_create);
		if (res != 0)
		{
			perror("t_sem_wait : pthread_rwlock_unlock of __lock_t_sem_create failed");
			exit(EXIT_FAILURE);
		}

		res = sem_wait(&__sems[numSem]);
		if (res != 0)
		{
			perror("t_sem_wait : Semaphore wait failed");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("t_sem_wait : Semaphore wait failed - semaphore number is out of range");
		exit(EXIT_FAILURE);
	}
}

void t_sem_post(int numSem)
{
	int res = pthread_rwlock_rdlock(&__lock_t_sem_create);
	if (res != 0)
	{
		perror("t_sem_post : pthread_rwlock_rdlock of __lock_t_sem_create failed");
		exit(EXIT_FAILURE);
	}
	if (numSem >= 0 && numSem < __countSem)
	{
		res = pthread_rwlock_unlock(&__lock_t_sem_create);
		if (res != 0)
		{
			perror("t_sem_post : pthread_rwlock_unlock of __lock_t_sem_create failed");
			exit(EXIT_FAILURE);
		}

		res = sem_post(&__sems[numSem]);
		if (res != 0)
		{
			perror("t_sem_post : Semaphore post failed");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("t_sem_post : Semaphore post failed - number of semaphore is out of range");
		exit(EXIT_FAILURE);
	}
}

void t_msg_send(struct msg_info msg)
{
	int res = pthread_rwlock_rdlock(&__lock_t_create);
	if (res != 0)
	{
		perror("t_msg_send : pthread_rwlock_rdlock of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}
	if (msg.numTh >= 0 && msg.numTh < __countTh)
	{
		struct __threadInfo *th_info = &(__threads[msg.numTh]);

		res = pthread_rwlock_unlock(&__lock_t_create);
		if (res != 0)
		{
			perror("t_msg_send : pthread_rwlock_unlock of __lock_t_create failed");
			exit(EXIT_FAILURE);
		}
		res = pthread_mutex_lock(&(th_info->lock));
		if (res != 0)
		{
			perror("t_msg_send : pthread_mutex_lock of th_info.lock failed");
			exit(EXIT_FAILURE);
		}
		if (th_info->countMsg >= __COUNT_MSGS_FOR_TH)
		{
			perror("t_msg_send : Trying to send too much messages");
			exit(EXIT_FAILURE);
		}
		th_info->msgs[th_info->countMsg].numTh = t_getThNum();
		th_info->msgs[th_info->countMsg++].data = msg.data;

		if (th_info->countMsg == 1)
		{
			res = pthread_cond_signal(&(th_info->cond));
			if (res != 0)
			{
				perror("t_msg_send : pthread_cond_signal of th_info.cond failed");
				exit(EXIT_FAILURE);
			}
		}
		res = pthread_mutex_unlock(&(th_info->lock));
		if (res != 0)
		{
			perror("t_msg_send : pthread_mutex_unlock of th_info.lock failed");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("t_msg_send : Message send failed - number of thread is out of range");
		exit(EXIT_FAILURE);
	}
}
struct msg_info t_msg_receive()
{
	int res = pthread_rwlock_rdlock(&__lock_t_create), numTh;
	struct msg_info msg;
	if (res != 0)
	{
		perror("t_msg_recieve : pthread_rwlock_rdlock of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}
	numTh = t_getThNum();
	struct __threadInfo *th_info = &(__threads[numTh]);

	res = pthread_rwlock_unlock(&__lock_t_create);
	if (res != 0)
	{
		perror("t_msg_recieve : pthread_rwlock_unlock of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_mutex_lock(&(th_info->lock));
	if (res != 0)
	{
		perror("t_msg_recieve : pthread_mutex_lock of th_info.lock failed");
		exit(EXIT_FAILURE);
	}
	if (th_info->countMsg == 0)
	{
		res = pthread_cond_wait(&(th_info->cond), &(th_info->lock));
		if (res != 0)
		{
			perror("t_msg_recieve : pthread_cond_wait of th_info.cond and th_info.lock failed");
			exit(EXIT_FAILURE);
		}
	}
	msg = th_info->msgs[--th_info->countMsg];
	res = pthread_mutex_unlock(&(th_info->lock));
	if (res != 0)
	{
		perror("t_msg_recieve : pthread_mutex_unlock of th_info.lock failed");
		exit(EXIT_FAILURE);
	}
	return msg;
}

void t_destroy()
{
	sem_wait(&utilsem);
	printf("executing t_destroy...\n");
	sem_post(&utilsem);
	int res, i;

	for (i = 0; i < __countTh; i++)
	{
		res = pthread_cond_destroy(&(__threads[i].cond));
		if (res != 0)
		{
			perror("t_destroy : pthread_cond_destroy of __threads[i].cond failed");
			exit(EXIT_FAILURE);
		}
		res = pthread_mutex_destroy(&(__threads[i].lock));
		if (res != 0)
		{
			perror("t_destroy : pthread_mutex_destroy of __threads[i].lock failed");
			exit(EXIT_FAILURE);
		}
	}
	for (i = 0; i < __countSem; i++)
	{
		res = sem_destroy(&(__sems[i]));
		//res = sem_close(__sems[i]);
		if (res != 0)
		{
			perror("t_destroy : sem_destroy of __sems[i] failed");
			exit(EXIT_FAILURE);
		}
	}
	res = pthread_rwlock_destroy(&__lock_t_create);
	if (res != 0)
	{
		perror("t_destroy : pthread_rwlock_destroy of __lock_t_create failed");
		exit(EXIT_FAILURE);
	}

	res = pthread_rwlock_destroy(&__lock_t_sem_create);
	if (res != 0)
	{
		perror("t_destroy : pthread_rwlock_destroy of __lock_t_sem_create failed");
		exit(EXIT_FAILURE);
	}

	sem_wait(&utilsem);
	printf("t_destroy done\n");
	sem_post(&utilsem);

	//sem_destroy(&utilsem);
}
