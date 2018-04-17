#include "dbuff.h"
#include "string.h"
// #include "uart.h"

#define DBUFF_MAX_SIZE (1024*8)
#define SEM_WAIT_TIME pdMS_TO_TICKS(10)

int dbuff_init(int buff_size, dbuff_t *dbuff){
	if(buff_size >= DBUFF_MAX_SIZE)
		return -1;
	if(buff_size < 0)
		return -2;

	dbuff->buff1 = (char*)malloc(buff_size);
	dbuff->buff2 = (char*)malloc(buff_size);
	dbuff->buff_size = buff_size;

	dbuff->read_buff = 0;
	dbuff->write_buff = 0;

	dbuff->read_ptr = 0;
	dbuff->write_ptr = 0;

	dbuff->buff1_sem = xSemaphoreCreateMutex();
	dbuff->buff2_sem = xSemaphoreCreateMutex();

	dbuff->read_started = 0;

	return 0;
}

void dbuff_destroy(dbuff_t *dbuff){
	if(!dbuff)
		return;

	free(dbuff->buff1);
	free(dbuff->buff2);

	vSemaphoreDelete(dbuff->buff1_sem);
	vSemaphoreDelete(dbuff->buff2_sem);
}

int dbuff_put(const char *data, size_t size, dbuff_t *dbuff){
	int can_write = size;
	int cuz_of_readptr = 0;

	SemaphoreHandle_t sem = dbuff->write_buff ? dbuff->buff2_sem : dbuff->buff1_sem;
	while(xSemaphoreTake(sem, SEM_WAIT_TIME) != pdTRUE)
		;

	if(dbuff->read_buff == dbuff->write_buff && dbuff->read_started && dbuff->read_ptr > dbuff->write_ptr){
		int limit = dbuff->read_ptr - dbuff->write_ptr - 1;

		if(limit < 0){
			xSemaphoreGive(sem);
			return 0;
		}

		if(can_write > limit)
			can_write = limit;
		cuz_of_readptr = 1;
	}

	int diff = dbuff->buff_size - dbuff->write_ptr;
	if(diff < can_write){
		can_write = diff;
		cuz_of_readptr = 0;
	}

	if(!can_write){
		xSemaphoreGive(sem);
		return 0;
	}

	char *dest = dbuff->write_buff ? dbuff->buff2 : dbuff->buff1;

	memcpy((void*)(dest + dbuff->write_ptr), (void*)data, can_write);
	dbuff->write_ptr += can_write;

	if(cuz_of_readptr){
		xSemaphoreGive(sem);
		return can_write;
	}

	size -= can_write;

	if(dbuff->write_ptr >= dbuff->buff_size){
		dbuff->write_ptr = 0;
		dbuff->write_buff = !dbuff->write_buff;
	}

	// uart << "Write ptr: " << dbuff->write_ptr << " Buff: " << dbuff->write_buff << endl;

	if(size){
		xSemaphoreGive(sem);
		return can_write + dbuff_put(data + can_write, size, dbuff);
	}

	xSemaphoreGive(sem);

	return can_write;
}

int dbuff_read(char *dest, size_t size, dbuff_t *dbuff){
	int can_read = size;
	int limit = 0;

	SemaphoreHandle_t sem = dbuff->read_buff ? dbuff->buff2_sem : dbuff->buff1_sem;

	while(xSemaphoreTake(sem, SEM_WAIT_TIME) != pdTRUE)
		;

	int read_ptr = dbuff->read_ptr;
	int write_ptr = dbuff->write_ptr;
	if(dbuff->read_buff == dbuff->write_buff && read_ptr <= write_ptr){
		if(read_ptr == write_ptr){
			xSemaphoreGive(sem);
			return 0;
		}

		limit = write_ptr - read_ptr;
		if(limit < 0)
			limit = 0;
		if(limit < can_read)
			can_read = limit;
	}

	limit = dbuff->buff_size - read_ptr;
	if(limit < can_read)
		can_read = limit;

	char *src = dbuff->read_buff ? dbuff->buff2 : dbuff->buff1;
	memcpy((void*)dest, (void*)(src + read_ptr), can_read);

	dbuff->read_ptr += can_read;

	// uart << "Read ptr: " << dbuff->read_ptr << " Buff: " << dbuff->read_buff << endl;	

	if(can_read)
		dbuff->read_started = 1;

	if(dbuff->read_ptr == dbuff->buff_size){
		dbuff->read_ptr = 0;
		dbuff->read_buff = !dbuff->read_buff;
		xSemaphoreGive(sem);
		return can_read + dbuff_read(dest + can_read, size - can_read, dbuff);
	}

	xSemaphoreGive(sem);
	return can_read;
}

uint32_t dbuff_get_buffered_data_length(dbuff_t *dbuff){
	if(xSemaphoreTake(dbuff->buff1_sem, SEM_WAIT_TIME) != pdTRUE)
		return 0;
	if(xSemaphoreTake(dbuff->buff2_sem, SEM_WAIT_TIME) != pdTRUE){
		xSemaphoreGive(dbuff->buff1_sem);
		return 0;
	}

	int length = 0;
	uint16_t readptr = dbuff->read_ptr;
	uint16_t writeptr = dbuff->write_ptr;

	if(dbuff->read_buff == dbuff->write_buff){
		length = writeptr > readptr ? writeptr - readptr : readptr - writeptr;
		xSemaphoreGive(dbuff->buff2_sem);
		xSemaphoreGive(dbuff->buff1_sem);

		return length;
	}

	length = writeptr + dbuff->buff_size - readptr;

	xSemaphoreGive(dbuff->buff2_sem);
	xSemaphoreGive(dbuff->buff1_sem);
	return length;
}

int dbuff_clear_buffer(dbuff_t *dbuff){
	if(xSemaphoreTake(dbuff->buff1_sem, SEM_WAIT_TIME) != pdTRUE)
		return -1;
	if(xSemaphoreTake(dbuff->buff2_sem, SEM_WAIT_TIME) != pdTRUE){
		xSemaphoreGive(dbuff->buff1_sem);
		return -1;
	}

	dbuff->write_ptr = 0;
	dbuff->read_ptr = 0;
	dbuff->write_buff = 0;
	dbuff->read_buff = 0;

	xSemaphoreGive(dbuff->buff2_sem);
	xSemaphoreGive(dbuff->buff1_sem);

	return 0;
}
