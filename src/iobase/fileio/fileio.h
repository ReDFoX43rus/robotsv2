#ifndef IOBASE_FILEIO_H
#define IOBASE_FILEIO_H

#include "../iobase.h"
#include "stdio.h"

enum FileMode{
	FM_READ = 1,
	FM_WRITE = 1 << 2,
	FM_RW = FM_READ|FM_WRITE,
	FM_APPEND = 1 << 3
};

class CFile : public CIOBase {
public:
	CFile(const char *filename, FileMode mode);
	~CFile();

	bool Open(const char *filename, FileMode mode);
	bool IsOpened() {return m_File != NULL;}
	void Close();

	uint32_t GetChar();
	size_t GetString(char *string, size_t size);

	size_t Write(const char *data, size_t size);

	size_t GetSize();
private:
	FILE* m_File;
	FileMode m_Mode;
};

#endif /* end of include guard: IOBASE_FILEIO_H */
