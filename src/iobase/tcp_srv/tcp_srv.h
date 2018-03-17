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
	CTcp(uint16_t port, uint32_t heartbeatDelay);
	~CTcp();

	/* Make tcp server listen on port */
	int Init();

	/* Accept new client and handle income data in a new thread
	 * Since this class currently made for 1 client
	 * this function shouldn't be called more than 1 time */
	int AcceptAndHandle();

	/* Destroys (and drops) client handler created by AcceptAndHandle */
	int DestroyClientHandler();

	/* Drop currently connected client without destroing handler
	 * So another client can connect */
	int DropClient();

	/* Clear m_IOQueue */
	void Flush() {m_QueueBack = 0; m_QueueFront = 0;}

	/* Create new console thread
	 * so we can send console cmds via tcp server */
	int SetupConsole();

	/* Destroy console task */
	void DestroyConsole();

	/* === IOBase functions === */
	uint32_t GetChar();
	size_t Write(const char *data, size_t size);
	int GetBufferedDataLength();
	int GetBytes(char *data, size_t size);
	/* === IOBase functions === */

	uint16_t GetPort() {return m_Port;}
private:
	uint16_t m_Port;
	int m_ServerSocket;
	int m_ConnectSocket;

	/* Max amount of time (seconds) that client can sleep
	 * After that client will be disconnected */
	uint32_t m_HeartbeatDelay;

	/* Timestamp (seconds) of last received chunk */
	uint32_t m_LastHeartbeat;
	SemaphoreHandle_t m_HeartbeatSem;

	/* Task descriptor */
	TaskHandle_t m_CheckHeartbeatTask;
	/* If last received chunk was too late then disconnect client */
	static void CheckHeartbeat(void *arg);

	/* Enqueue data received from client
	 * Dequeue data requested by console */
	SemaphoreHandle_t m_BuffSem;
	char m_IOQueue[TCPIO_MAX_BUFF_SIZE];
	uint32_t m_QueueFront;
	uint32_t m_QueueBack;

	/* Shift queue pointers as left as possible
	 * Must be called only when semaphore is taken */
	void BalanceQueue();

	/* Task's descrptors and handlers */
	TaskHandle_t m_ConsoleTask;
	static void AttachToConsole(void *arg);

	TaskHandle_t m_ClientHandleTask;
	static void HandleClient(void *arg);

	static int show_socket_error_reason(const char *str, int socket);
	static int get_socket_error_code(int socket);
};

#endif /* end of include guard: IOBASE_TCPSRV_TCPSRV_H */
