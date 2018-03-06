#include "fileio.h"

CFile::CFile(const char *filename, FileMode mode){
	Open(filename, mode);
}

CFile::~CFile(){
	Close();
}

bool CFile::Open(const char *filename, FileMode mode){
	if(mode & FM_APPEND){
		if(mode & FM_READ)
			m_File = fopen(filename, "a+");
		else
			m_File = fopen(filename, "a");
	} else if(mode & FM_WRITE){
		if(mode & FM_READ)
			m_File = fopen(filename, "w+");
		else
			m_File = fopen(filename, "w");
	} else if(mode & FM_READ)
		m_File = fopen(filename, "r");

	if(m_File)
		fseek(m_File, 0, SEEK_SET);
	return m_File != NULL;
}

uint32_t CFile::GetChar(){
	if(!m_File)
		return 0;

	char c;
	int bytes = fread(&c, 1, 1, m_File);
	if(!bytes)
		return EOF;
	return c;
}

size_t CFile::GetString(char *string, size_t size){
	if(!m_File)
		return 0;

	return fread(string, 1, size, m_File);
}

size_t CFile::Write(const char *data, size_t size){
	if(m_File)
		return fwrite(data, size, 1, m_File);
	return -1;
}

void CFile::Close(){
	if(m_File){
		fclose(m_File);
		m_File = NULL;
	}
}

size_t CFile::GetSize(){
	if(!m_File)
		return 0;

	size_t currentPos = ftell(m_File);
	fseek(m_File, 0, SEEK_END);
	size_t size = ftell(m_File);

	fseek(m_File, currentPos, SEEK_SET);
	return size;
}

int CFile::GetBufferedDataLength() {return GetSize();};
int CFile::GetBytes(char *data, size_t size) {return GetString(data, size);}
