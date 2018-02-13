#ifndef IOBASE_WIFIIO_WIFIIO_H
#define IOBASE_WIFIIO_WIFIIO_H

#include "iobase/iobase.h"
#include "cpthread/rwlock.h"
#include "semaphore.h"

#define WRITE_BUFF_SIZE 1024
#define READ_BUFF_SIZE 1024

class CWifiIO : public CIOBase{
public:
	enum WriteFlags{
		WRITE_BUFF_CAN_BE_READ,
		WRITE_BUFF_CANT_BE_READ
	};

	CWifiIO();
	~CWifiIO();

	/* IOBase virtual functions */
	uint32_t GetChar();
	size_t Write(const char *data, size_t size);

	void SetWriteFlag(WriteFlags flag);
	WriteFlags GetWriteFlag();

	void FlushReadBuffer();
	void FlushWriteBuffer();

	/* === These functions must be called from CWfifi class only === */
	/* Append m_ReadBuffer */
	size_t WriteToReadBuff(const char *data, size_t size);
	size_t ReadFromWriteBuff(char *to, size_t size);

	/* If was error during read from write buff */
	/* No semaphore since we are expecting this function to be called from CWifi class only */
	int m_Error;
	/* =============================================================== */
private:
	uint32_t m_ReadBuffPointer; // points to current position in m_ReadBuffer
	uint32_t m_WriteBuffPointer; // points to current position in m_WriteBuffer

	/* It should be understanded as "read from wifi"  and "write to wifi" */
	char m_ReadBuffer[READ_BUFF_SIZE];
	char m_WriteBuffer[WRITE_BUFF_SIZE];

	pthread_rwlock_t m_ReadBuffLock;
	pthread_rwlock_t m_WriteBuffLock;
	sem_t m_FlagSem;

	/* If it's set to WRITE_BUFF_CAN_BE_READ we are not expecting to m_WriteBuffer to be appended */
	/* So if we read from this buff till the end, we will understand that we have read all we needed */
	/* If this flag is set to WRITE_BUFF_CANT_BE_READ and we've read m_WriteBuffer till the end, we have to wait when this buffer will be appended */
	WriteFlags m_WriteFlag;
};

#endif /* end of include guard: IOBASE_WIFIIO_WIFIIO_H */
