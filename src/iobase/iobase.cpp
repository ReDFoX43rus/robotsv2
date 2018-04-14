#include "iobase.h"
#include "fileio/fileio.h"
#include <cstdlib>
#include <cstring>
#include "uart.h"

#define STRING_BUFF_SIZE 512
#define INT_BUFF_SIZE 16

CIOBase::~CIOBase(){}
uint32_t CIOBase::GetChar(){return 0;}
uint32_t CIOBase::Write(const char *data, uint32_t size) {return 0;}

bool CIOBase::GetString(char *string, size_t size){
	bool retVal = true;
	size_t from = 0;
	int reason = 1;

	while((retVal = GetWord(string, size, from, reason)) && reason == 1)
		;

	return retVal;
}

char *CIOBase::GetString(){
	char *str = (char*)malloc(STRING_BUFF_SIZE);
	GetString(str, STRING_BUFF_SIZE);
	return str;
}

bool CIOBase::GetWord(char *string, size_t size, size_t &from, int &reason){
	if(from >= size)
		return false;

	if(from > 0)
		string[from -1] = ' ';

	size_t counter = from;
	bool retVal = true;

	uint32_t c = GetChar();
	while(c != '\r' && c != '\n' && c != ' ' && c != EOF){
		string[counter++] = (char)c;

		*this << (char)c;

		// backspace
		if(c == '\b'){
			if(counter > 1){
				*this << ' ';
				*this << '\b';
				counter -= 2;
			}
			else counter = 0;
		}

		c = GetChar();

		if(counter >= size - 1){
			retVal = false;
			break;
		}
	}

	switch(c){
		case ' ':
			reason = 1;
			*this << ' ';
			break;
		default:
			reason = 0;
	}

	from = counter;

	string[counter] = '\0';
	from = counter + 1;
	return retVal;
}

CIOBase &CIOBase::operator<<(char c){
	Write(&c, 1);
	return *this;
}

char CIOBase::operator>>(char &c){
	c = GetChar();
	return c;
}

CIOBase &CIOBase::operator<<(const char *data){
	Write(data, strlen(data));
	return *this;
}

CIOBase &CIOBase::operator<<(int data){
	if(data == 0){
		*this << '0';
		return *this;
	}

	if(data < 0){
		*this << '-';
		data *= -1;
	}

	char tmp[16];
	int counter = 0;
	int t;
	while(data){
		t = data % 10;
		data /= 10;

		tmp[counter++] = t + '0';
	}

	counter--;
	while(counter != -1){
		*this << tmp[counter--];
	}

	return *this;
}

CIOBase &CIOBase::operator>>(int &data){
	char *str = (char*)malloc(INT_BUFF_SIZE);
	char *pEnd;

	if(GetWord(str, INT_BUFF_SIZE)){
		data = (int)strtol(str, &pEnd, 10);
	} else data = 0;

	free(str);
	return *this;
}

CIOBase &CIOBase::operator<<(uint32_t data){
	char tmp[16];
	int counter = 0;
	int t;
	while(data){
		t = data % 10;
		data /= 10;

		tmp[counter++] = t + '0';
	}

	counter--;
	while(counter != -1){
		*this << tmp[counter--];
	}

	return *this;
}

CIOBase &CIOBase::operator>>(uint32_t &data){
	char *str = (char*)malloc(INT_BUFF_SIZE);
	char *pEnd;

	if(GetWord(str, INT_BUFF_SIZE)){
		data = (uint32_t)strtoul(str, &pEnd, 10);
	} else data = 0;

	free(str);
	return *this;
}

int CIOBase::GetBufferedDataLength() {return 0;}
int CIOBase::GetBytes(char *data, size_t size) { return 0;}
