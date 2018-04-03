#ifndef UTIL_DOUBLEBUFFER_DBUFF_H
#define UTIL_DOUBLEBUFFER_DBUFF_H

typedef struct {
	char *buff1;
	char *buff2;
	int buff_size;
	int read_buff;

	int read_ptr;
	int write_ptr;
} dbuff_t;

int dbuff_init(int buff_size, dbuff_t *dbuff);
void dbuff_destroy(dbuff_t *dbuff);

int dbuff_put(const char *data, int size, dbuff_t *dbuff);
int dbuff_read(char *dest, int size, dbuff_t *dbuff);

#endif /* end of include guard: UTIL_DOUBLEBUFFER_DBUFF_H */
