#include "wifiio.h"
#include "stdio.h"
#include "string.h"

#include "uart.h"

CWifiIO::CWifiIO(){
	pthread_rwlock_init(&m_ReadBuffLock, NULL);
	pthread_rwlock_init(&m_WriteBuffLock, NULL);
	sem_init(&m_FlagSem, 1, 0);
}
CWifiIO::~CWifiIO(){
	pthread_rwlock_destroy(&m_ReadBuffLock);
	pthread_rwlock_destroy(&m_WriteBuffLock);
	sem_destroy(&m_FlagSem);
}

uint32_t CWifiIO::GetChar(){
	pthread_rwlock_rdlock(&m_ReadBuffLock);
	if(m_ReadBuffPointer <= 0){
		pthread_rwlock_unlock(&m_ReadBuffLock);

		// We have to free rwlock sometimes in case of infinity loop
		// Let reader read something ;)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		return EOF;
	}

	char result = m_ReadBuffer[0];
	m_ReadBuffPointer--;

	memmove(m_ReadBuffer, m_ReadBuffer + 1, m_ReadBuffPointer);
	pthread_rwlock_unlock(&m_ReadBuffLock);

	return (uint32_t)result;
}

size_t CWifiIO::Write(const char *data, size_t size){
	pthread_rwlock_wrlock(&m_WriteBuffLock);

	if(size > WRITE_BUFF_SIZE - m_WriteBuffPointer)
		size = WRITE_BUFF_SIZE - m_WriteBuffPointer;

	memcpy(m_WriteBuffer + m_WriteBuffPointer, data, size);

	m_WriteBuffPointer += size;

	pthread_rwlock_unlock(&m_WriteBuffLock);

	return size;
}

size_t CWifiIO::WriteToReadBuff(const char *data, size_t size){
	pthread_rwlock_wrlock(&m_ReadBuffLock);

	size = size > READ_BUFF_SIZE - m_ReadBuffPointer ? READ_BUFF_SIZE - m_ReadBuffPointer : size;

	memcpy(m_ReadBuffer + m_ReadBuffPointer, data, size);
	m_ReadBuffPointer += size;

	uart << "Writed to read buff: ";
	uart.Write(data, size);
	uart << endl;

	pthread_rwlock_unlock(&m_ReadBuffLock);
	return size;
}

size_t CWifiIO::ReadFromWriteBuff(char *to, size_t size){
	pthread_rwlock_rdlock(&m_WriteBuffLock);

	if(size > m_WriteBuffPointer)
		size = m_WriteBuffPointer;

	memcpy(to, m_WriteBuffer, size);
	m_WriteBuffPointer -= size;

	pthread_rwlock_unlock(&m_WriteBuffLock);
	return size;
}

void CWifiIO::SetWriteFlag(WriteFlags flag){
	sem_wait(&m_FlagSem);
	m_WriteFlag = flag;
	sem_post(&m_FlagSem);
}

CWifiIO::WriteFlags CWifiIO::GetWriteFlag(){
	WriteFlags result;

	sem_wait(&m_FlagSem);
	result = m_WriteFlag;
	sem_post(&m_FlagSem);

	return result;
}

void CWifiIO::FlushReadBuffer(){
	pthread_rwlock_wrlock(&m_ReadBuffLock);
	m_ReadBuffPointer = 0;
	pthread_rwlock_unlock(&m_ReadBuffLock);
}
void CWifiIO::FlushWriteBuffer(){
	pthread_rwlock_wrlock(&m_WriteBuffLock);
	m_WriteBuffPointer = 0;
	pthread_rwlock_unlock(&m_WriteBuffLock);
}
