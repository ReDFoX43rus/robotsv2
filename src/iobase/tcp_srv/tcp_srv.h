#ifndef IOBASE_TCPSRV_TCPSRV_H
#define IOBASE_TCPSRV_TCPSRV_H

#include "iobase/iobase.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TCPIO_MAX_BUFF_SIZE (4096*2)
/* TCPIO_RECV_BUFF_SIZE should be like 1/8 of TCPIO_MAX_BUFF_SIZE */
#define TCPIO_RECV_BUFF_SIZE 512
#define TCPIO_SEM_WAIT_TIME pdMS_TO_TICKS(100)

class CTcp : public CIOBase{
public:
	CTcp(uint16_t port);
	~CTcp();

	int Init();
	int AcceptAndHandle();
	int DestroyClientHandler();
	int DropClient();
	void Flush() {m_QueueBack = 0; m_QueueFront = 0;}

	int SetupConsole();
	void DestroyConsole();

	/* IOBase functions */
	uint32_t GetChar();
	size_t Write(const char *data, size_t size);
	int GetBufferedDataLength();
	int GetBytes(char *data, size_t size);

	uint16_t GetPort() {return m_Port;}
private:
	uint16_t m_Port;
	int m_ServerSocket;
	int m_ConnectSocket;

	SemaphoreHandle_t m_BuffSem;
	char m_IOQueue[TCPIO_MAX_BUFF_SIZE];
	uint32_t m_QueueFront;
	uint32_t m_QueueBack;
	/* Must be called only when semaphore is taken */
	void BalanceQueue();

	TaskHandle_t m_ConsoleTask;
	static void AttachToConsole(void *arg);

	TaskHandle_t m_ClientHandleTask;
	static void HandleClient(void *arg);

	static int show_socket_error_reason(const char *str, int socket);
	static int get_socket_error_code(int socket);
};

#endif /* end of include guard: IOBASE_TCPSRV_TCPSRV_H */
