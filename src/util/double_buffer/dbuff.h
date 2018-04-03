#ifndef UTIL_DOUBLEBUFFER_DBUFF_H
#define UTIL_DOUBLEBUFFER_DBUFF_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "inttypes.h"

typedef struct {
	char *buff1;
	char *buff2;
	uint16_t buff_size;

	uint16_t read_ptr;
	uint16_t write_ptr;

	uint8_t read_buff;
	uint8_t write_buff;

	SemaphoreHandle_t buff1_sem;
	SemaphoreHandle_t buff2_sem;
} dbuff_t;

int dbuff_init(int buff_size, dbuff_t *dbuff);
void dbuff_destroy(dbuff_t *dbuff);

int dbuff_put(const char *data, size_t size, dbuff_t *dbuff);
int dbuff_read(char *dest, size_t size, dbuff_t *dbuff);

uint32_t dbuff_get_buffered_data_length(dbuff_t *dbuff);
int dbuff_clear_buffer(dbuff_t *dbuff);

#endif /* end of include guard: UTIL_DOUBLEBUFFER_DBUFF_H */
