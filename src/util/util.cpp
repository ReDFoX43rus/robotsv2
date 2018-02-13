#include "stddef.h"
#include "string.h"
#include "util.h"

/* Splt string. Expects that string ends with \0 */
splited_string_t string_split(char *data){
	char c;
	size_t spaces = 0;

	for(int i = 0; (c = data[i]) != '\0'; i++){
		if(c == ' ')
			spaces++;
	}

	char **arr = new char*[spaces + 1];

	size_t ptr = 0;
	for(size_t i = 0; i <= spaces; i++){
		size_t wordLength = 0;

		while((c = data[ptr]) != ' ' && c != '\0'){
			wordLength++;
			ptr++;
		}

		arr[i] = new char[wordLength + 1];//malloc(wordLength + 1);
		arr[i][wordLength] = '\0';
		memcpy(arr[i], data + ptr - wordLength, wordLength);

		ptr++;
	}

	splited_string_t result = {
		.data = arr,
		.size = spaces + 1
	};

	return result;
}

void release_split(splited_string_t source){
	for(size_t i = 0; i < source.size; i++){
		delete[] source.data[i];
	}

	delete[] source.data;
}
