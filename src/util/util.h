typedef struct splitedString{
	char **data;
	size_t size;
} splited_string_t;

extern splited_string_t string_split(char *data);
extern void release_split(splited_string_t source);
