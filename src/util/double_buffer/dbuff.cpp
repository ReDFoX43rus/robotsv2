#include "dbuff.h"

#define DBUFF_MAX_SIZE (1024*8)

int dbuff_init(int buff_size, dbuff_t *dbuff){
	if(buff_size >= DBUFF_MAX_SIZE)
		return -1;
	if(buff_size < 0)
		return -2;

	dbuff->buff1 = (char*)malloc(buff_size);
	dbuff->buff2 = (char*)malloc(buff_size);
	dbuff->buff_size = buff_size;
	dbuff->read_buff = 0;

	dbuff->read_ptr = 0;
	dbuff->write_ptr = 0;

	return 0;
}

void dbuff_destroy(dbuff_t *dbuff){
	if(!debuff)
		return;

	free(dbuff->buff1);
	free(dbuff->buff2);
}

int dbuff_put(const char *data, int size, dbuff_t *dbuff){

}

int dbuff_read(char *dest, int size, dbuff_t *dbuff);
