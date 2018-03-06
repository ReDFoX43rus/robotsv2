#ifndef IOBASE_IOBASE_H
#define IOBASE_IOBASE_H

#include "inttypes.h"
#include "stdlib.h"

/* @brief base class for io communications */
class CIOBase{
public:
	virtual ~CIOBase();

	/* Returns char, blocks input */
	virtual uint32_t GetChar();

	/* Returns true on success */
	bool GetString(char *string, size_t size);
	/* Backward compatibility */
	char *GetString();
	/* Get word */
	bool GetWord(char *string, size_t size) {
		size_t from = 0;
		int reason = 0;
		return GetWord(string, size, from, reason);
	}

	/* Returns number of written bytes */
	virtual size_t Write(const char *data, size_t size);

	/* Read and write overloads */
	CIOBase &operator<<(char c);
	char operator>>(char &c);

	CIOBase &operator<<(const char *data);

	CIOBase &operator<<(int data);
	CIOBase &operator>>(int &data);

	CIOBase &operator<<(uint32_t data);
	CIOBase &operator>>(uint32_t &data);

private:
	bool GetWord(char *string, size_t size, size_t &from, int &reason);
};

static const char endl[] = "\r\n";

#endif /* end of include guard: IOBASE_IOBASE_H */
