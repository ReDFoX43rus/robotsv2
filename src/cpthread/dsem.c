#include "dsem.h"
#include "freertos/task.h"

dsem_t *dsem_init(int count, int limit){
	dsem_t *dsem = (dsem_t*)malloc(sizeof(dsem_t));
	dsem->count = 0;
	dsem->limit = limit;
	sem_init(&dsem->sem, 1, 0);

	for(int i = 0; i < count; i++)
		dsem_v(dsem);

	return dsem;
}

void dsem_p(dsem_t *dsem){
	while(1){
		// This should be atomic
		sem_wait(&dsem->sem);
		if(dsem->count){
			dsem->count--;
			break;
		}
		sem_post(&dsem->sem);

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
void dsem_v(dsem_t *dsem){
	// This sould be atomic
	sem_wait(&dsem->sem);
	if(dsem->count < dsem->limit)
		dsem->count++;
	sem_post(&dsem->sem);
}

void dsem_destroy(dsem_t *dsem){
	sem_destroy(&dsem->sem);
	free(dsem);
}
