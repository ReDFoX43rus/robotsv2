#include "pthread.h"

#ifndef INCLUDE_xTaskGetCurrentTaskHandle
#define INCLUDE_xTaskGetCurrentTaskHandle
#endif

#ifndef INCLUDE_vTaskDelete
#define INCLUDE_vTaskDelete
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void pthread_exit(void *retval){
	TaskHandle_t handle = xTaskGetCurrentTaskHandle();
	if(handle)
		vTaskDelete(handle);
}
