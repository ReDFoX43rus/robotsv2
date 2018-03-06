#ifndef IOBASE_TCPSRV_TCPSRV_H
#define IOBASE_TCPSRV_TCPSRV_H

#include "iobase/iobase.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TCPIO_MAX_BUFF_SIZE 2048
#define TCPIO_RECV_BUFF_SIZE 64
#define TCPIO_SEM_WAIT_TIME pdMS_TO_TICKS(100)

class CTcp : public CIOBase{
public:
	CTcp(uint16_t port);
	~CTcp();

	int Init();
	int AcceptAndRecv();
	void Flush() {m_CurrentBuffSize = 0;}

	/* IOBase functions */
	uint32_t GetChar();
	size_t Write(const char *data, size_t size);

private:
	uint16_t m_Port;
	int m_ServerSocket;
	int m_ConnectSocket;

	SemaphoreHandle_t m_BuffSem;
	char m_IOBuffer[TCPIO_MAX_BUFF_SIZE];
	uint32_t m_CurrentBuffSize;

	static int show_socket_error_reason(const char *str, int socket);
	static int get_socket_error_code(int socket);
};

#endif /* end of include guard: IOBASE_TCPSRV_TCPSRV_H */
