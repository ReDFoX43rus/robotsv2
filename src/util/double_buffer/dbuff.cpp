#include "dbuff.h"
#include "string.h"
// #include "uart.h"

#define DBUFF_MAX_SIZE (1024*8)
#define SEM_WAIT_TIME pdMS_TO_TICKS(10)

#define RAND_DELAY() vTaskDelay(pdMS_TO_TICKS(5 + rand()%15))

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

	dbuff->read_sem = xSemaphoreCreateMutex();
	dbuff->write_sem = xSemaphoreCreateMutex();

	dbuff->read_started = 0;

	return 0;
}

void dbuff_destroy(dbuff_t *dbuff){
	if(!dbuff)
		return;

	free(dbuff->buff1);
	free(dbuff->buff2);

	vSemaphoreDelete(dbuff->read_sem);
	vSemaphoreDelete(dbuff->write_sem);
}

int dbuff_put(const char *data, size_t size, dbuff_t *dbuff){
	int can_write = size;
	int cuz_of_readptr = 0;

	uint16_t read_ptr;
	uint8_t read_buff;
	uint8_t read_started;

	while(xSemaphoreTake(dbuff->read_sem, SEM_WAIT_TIME) != pdTRUE)
		RAND_DELAY();

	read_ptr = dbuff->read_ptr;
	read_buff = dbuff->read_buff;
	read_started = dbuff->read_started;

	xSemaphoreGive(dbuff->read_sem);

	uint16_t write_ptr;
	uint8_t write_buff;
	
	while(xSemaphoreTake(dbuff->write_sem, SEM_WAIT_TIME) != pdTRUE)
		RAND_DELAY();

	write_ptr = dbuff->write_ptr;
	write_buff = dbuff->write_buff;

	xSemaphoreGive(dbuff->write_sem);

	if(read_buff == write_buff && read_started && read_ptr > write_ptr){
		int limit = read_ptr - write_ptr - 1;

		if(limit < 0){
			return 0;
		}

		if(can_write > limit)
			can_write = limit;
		cuz_of_readptr = 1;
	}

	int diff = dbuff->buff_size - write_ptr;
	if(diff < can_write){
		can_write = diff;
		cuz_of_readptr = 0;
	}

	if(!can_write){
		return 0;
	}

	char *dest = write_buff ? dbuff->buff2 : dbuff->buff1;

	memcpy((void*)(dest + write_ptr), (void*)data, can_write);
	write_ptr += can_write;

	while(xSemaphoreTake(dbuff->write_sem, SEM_WAIT_TIME) != pdTRUE)
		RAND_DELAY();

	dbuff->write_ptr = write_ptr;

	if(cuz_of_readptr){
		xSemaphoreGive(dbuff->write_sem);
		return can_write;
	}

	size -= can_write;

	if(dbuff->write_ptr >= dbuff->buff_size){
		dbuff->write_ptr = 0;
		dbuff->write_buff = !dbuff->write_buff;
	}

	xSemaphoreGive(dbuff->write_sem);

	// uart << "Write ptr: " << dbuff->write_ptr << " Buff: " << dbuff->write_buff << endl;

	if(size){
		return can_write + dbuff_put(data + can_write, size, dbuff);
	}

	return can_write;
}

int dbuff_read(char *dest, size_t size, dbuff_t *dbuff){
	int can_read = size;
	int limit = 0;

	uint16_t read_ptr;
	uint8_t read_buff;

	while(xSemaphoreTake(dbuff->read_sem, SEM_WAIT_TIME) != pdTRUE)
		RAND_DELAY();

	read_ptr = dbuff->read_ptr;
	read_buff = dbuff->read_buff;

	xSemaphoreGive(dbuff->read_sem);

	uint16_t write_ptr;
	uint8_t write_buff;
	
	while(xSemaphoreTake(dbuff->write_sem, SEM_WAIT_TIME) != pdTRUE)
		RAND_DELAY();

	write_ptr = dbuff->write_ptr;
	write_buff = dbuff->write_buff;

	xSemaphoreGive(dbuff->write_sem);

	if(read_buff == write_buff && read_ptr <= write_ptr){
		if(read_ptr == write_ptr){
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

	char *src = read_buff ? dbuff->buff2 : dbuff->buff1;
	memcpy((void*)dest, (void*)(src + read_ptr), can_read);

	read_ptr += can_read;

	// uart << "Read ptr: " << dbuff->read_ptr << " Buff: " << dbuff->read_buff << endl;	

	while(xSemaphoreTake(dbuff->read_sem, SEM_WAIT_TIME) != pdTRUE)
		RAND_DELAY();

	dbuff->read_ptr = read_ptr;

	if(can_read)
		dbuff->read_started = 1;

	if(dbuff->read_ptr == dbuff->buff_size){
		dbuff->read_ptr = 0;
		dbuff->read_buff = !dbuff->read_buff;
		xSemaphoreGive(dbuff->read_sem);
		return can_read + dbuff_read(dest + can_read, size - can_read, dbuff);
	}

	xSemaphoreGive(dbuff->read_sem);
	return can_read;
}

uint32_t dbuff_get_buffered_data_length(dbuff_t *dbuff){
	if(xSemaphoreTake(dbuff->read_sem, SEM_WAIT_TIME) != pdTRUE)
		return 0;
	if(xSemaphoreTake(dbuff->write_sem, SEM_WAIT_TIME) != pdTRUE){
		xSemaphoreGive(dbuff->read_sem);
		return 0;
	}

	int length = 0;
	uint16_t readptr = dbuff->read_ptr;
	uint16_t writeptr = dbuff->write_ptr;

	if(dbuff->read_buff == dbuff->write_buff){
		length = writeptr > readptr ? writeptr - readptr : readptr - writeptr;
		xSemaphoreGive(dbuff->write_sem);
		xSemaphoreGive(dbuff->read_sem);

		return length;
	}

	length = writeptr + dbuff->buff_size - readptr;

	xSemaphoreGive(dbuff->write_sem);
	xSemaphoreGive(dbuff->read_sem);
	return length;
}

int dbuff_clear_buffer(dbuff_t *dbuff){
	if(xSemaphoreTake(dbuff->read_sem, SEM_WAIT_TIME) != pdTRUE)
		return -1;
	if(xSemaphoreTake(dbuff->write_sem, SEM_WAIT_TIME) != pdTRUE){
		xSemaphoreGive(dbuff->read_sem);
		return -1;
	}

	dbuff->write_ptr = 0;
	dbuff->read_ptr = 0;
	dbuff->write_buff = 0;
	dbuff->read_buff = 0;

	xSemaphoreGive(dbuff->write_sem);
	xSemaphoreGive(dbuff->read_sem);

	return 0;
}
