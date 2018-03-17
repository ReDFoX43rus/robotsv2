#include "tcp_srv.h"

#include <sys/socket.h>
#include "esp_log.h"
#include "string.h"
#include "time.h"
#include "uart.h"
#include "cconsole/console.h"

CTcp::CTcp(uint16_t port, uint32_t heartbeatDelay){
	m_Port = port;
	m_ServerSocket = 0;
	m_ConnectSocket = 0;
	m_QueueFront = 0;
	m_QueueBack = 0;
	m_BuffSem = xSemaphoreCreateMutex();
	m_ConsoleTask = 0;
	m_ClientHandleTask = 0;
	m_HeartbeatDelay = heartbeatDelay;
	m_LastHeartbeat = 0;
	m_CheckHeartbeatTask = 0;
	m_HeartbeatSem = 0;

	uart << "heartbeatDelay: " << heartbeatDelay << endl;
	if(heartbeatDelay){
		m_HeartbeatSem = xSemaphoreCreateMutex();
		xTaskCreate(CheckHeartbeat, "tcp_check_heartbeat", 2048, this, 1, &m_CheckHeartbeatTask);
	}
}

CTcp::~CTcp(){
	DestroyClientHandler();
	DestroyConsole();

	if(m_CheckHeartbeatTask)
		vTaskDelete(m_CheckHeartbeatTask);

	if(m_ConnectSocket)
		close(m_ConnectSocket);
	if(m_ServerSocket)
		close(m_ServerSocket);

	vSemaphoreDelete(m_BuffSem);

	if(m_HeartbeatDelay)
		vSemaphoreDelete(m_HeartbeatSem);
}

int CTcp::Init(){
	struct sockaddr_in server_addr;

	uart << "Starting tcp server on port: " << m_Port << endl;

	m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ServerSocket < 0) {
		show_socket_error_reason("create_server", m_ServerSocket);
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(m_Port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_ServerSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		show_socket_error_reason("bind_server", m_ServerSocket);
		close(m_ServerSocket);
		return -2;
	}

	uart << "Tcp server is listening..." << endl;

	if (listen(m_ServerSocket, 5) < 0) {
		show_socket_error_reason("listen_server", m_ServerSocket);
		close(m_ServerSocket);
		return -3;
	}

	return 0;
}

void CTcp::HandleClient(void *arg){
	CTcp *tcp = (CTcp*)arg;
	while(1){
		struct sockaddr_in client_addr;
		socklen_t socklen = sizeof(client_addr);

		// uart << "Tcp server is waiting for a client..." << endl;

		tcp->m_ConnectSocket = accept(tcp->m_ServerSocket, (struct sockaddr *)&client_addr, &socklen);

		if (tcp->m_ConnectSocket < 0) {
			show_socket_error_reason("accept_server", tcp->m_ConnectSocket);
			close(tcp->m_ServerSocket);
			tcp->m_ClientHandleTask = 0;
			tcp->m_ConnectSocket = 0;
			vTaskDelete(NULL);
		}

		/*connection established，now can send/recv*/
		// uart << "Client accepted" << endl;

		char buff[TCPIO_RECV_BUFF_SIZE];
		int len, to_write;
		int currentBuffSize = 0;

		while((len = recv(tcp->m_ConnectSocket, buff, TCPIO_RECV_BUFF_SIZE, 0)) > 0){

			/* prevent from losing data */
			while(1){

				while(xSemaphoreTake(tcp->m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
					vTaskDelay(pdMS_TO_TICKS(50));

				if(tcp->m_QueueFront + len > TCPIO_MAX_BUFF_SIZE)
					tcp->BalanceQueue();

				to_write = tcp->m_QueueFront + len <= TCPIO_MAX_BUFF_SIZE ? len : TCPIO_MAX_BUFF_SIZE - tcp->m_QueueFront;

				/* prevent from loosing data */
				if(len != to_write){
					xSemaphoreGive(tcp->m_BuffSem);
					vTaskDelay(pdMS_TO_TICKS(50));
					continue;
				}

				memcpy(tcp->m_IOQueue + tcp->m_QueueFront, buff, to_write);
				tcp->m_QueueFront += to_write;

				currentBuffSize = tcp->m_QueueFront - tcp->m_QueueBack;

				xSemaphoreGive(tcp->m_BuffSem);
				if(currentBuffSize == TCPIO_MAX_BUFF_SIZE)
					vTaskDelay(pdMS_TO_TICKS(350));

				break;
			}

			if(tcp->m_HeartbeatDelay && tcp->m_HeartbeatSem && xSemaphoreTake(tcp->m_HeartbeatSem, TCPIO_SEM_WAIT_TIME) == pdTRUE){
				tcp->m_LastHeartbeat = clock() / CLOCKS_PER_SEC;
				xSemaphoreGive(tcp->m_HeartbeatSem);
			}
		}
	}
}

int CTcp::AcceptAndHandle(){
	if(m_ClientHandleTask != 0)
		return -1;

	return xTaskCreate(HandleClient, "tcp_client_handle", 4096, this, 4, &m_ClientHandleTask) != pdTRUE ? -2 : 0;
}

int CTcp::DestroyClientHandler(){
	if(m_ClientHandleTask == 0)
		return -1;

	vTaskDelete(m_ClientHandleTask);
	DropClient();
	return 0;
}

int CTcp::DropClient(){
	if(m_ClientHandleTask == 0)
		return -1;

	close(m_ConnectSocket);
	Flush();

	if(m_HeartbeatSem){
		xSemaphoreTake(m_HeartbeatSem, TCPIO_SEM_WAIT_TIME);
		m_LastHeartbeat = 0;
		xSemaphoreGive(m_HeartbeatSem);
	}

	return 0;
}

void CTcp::AttachToConsole(void *arg){
	while(1)
		console.WaitForCmd(*(CTcp*)arg);
}

int CTcp::SetupConsole(){
	if(m_ConsoleTask != 0)
		return -2;

	return xTaskCreate(AttachToConsole, "tcp_console", 4096, this, 4, &m_ConsoleTask) != pdTRUE ? -1 : 0;
}

void CTcp::DestroyConsole(){
	if(m_ConsoleTask){
		vTaskDelete(m_ConsoleTask);
		m_ConsoleTask = 0;
	}
}

uint32_t CTcp::GetChar(){
	char ret;
	while(1){
		while(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
			;

		if(m_QueueBack == m_QueueFront){
			xSemaphoreGive(m_BuffSem);
			vTaskDelay(pdMS_TO_TICKS(50));
			continue;
		}

		ret = m_IOQueue[m_QueueBack++];

		xSemaphoreGive(m_BuffSem);
		break;
	}

	return ret;
}

size_t CTcp::Write(const char *data, size_t size){
	if(!m_ConnectSocket)
		return 0;

	int res = send(m_ConnectSocket, data, size, 0);

	return res >= 0 ? res : 0;
}

int CTcp::GetBufferedDataLength() {
	while(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
		;

	int len = m_QueueFront - m_QueueBack;

	xSemaphoreGive(m_BuffSem);

	return len;
}
int CTcp::GetBytes(char *data, size_t size){
	while(1){
		while(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
			;

		int buffSize = m_QueueFront - m_QueueBack;
		if(!buffSize){
			xSemaphoreGive(m_BuffSem);
			vTaskDelay(pdMS_TO_TICKS(50));
			continue;
		}

		if(size > buffSize)
			size = buffSize;

		memcpy(data, m_IOQueue + m_QueueBack, size);
		m_QueueBack += size;

		xSemaphoreGive(m_BuffSem);
		break;
	}
	return size;
}

void CTcp::BalanceQueue(){
	for(int i = m_QueueBack; i <= m_QueueFront; i++)
		m_IOQueue[i - m_QueueBack] = m_IOQueue[i];

	m_QueueFront -= m_QueueBack;
	m_QueueBack = 0;
}

void CTcp::CheckHeartbeat(void *arg){
	CTcp *tcp = (CTcp*)arg;

	while(1){
		/* Check if clients havent connected yet */
		if(!tcp->m_LastHeartbeat){
			vTaskDelay(pdMS_TO_TICKS(tcp->m_HeartbeatDelay));
			continue;
		}

		uint32_t now = clock() / CLOCKS_PER_SEC;
		if(now - tcp->m_LastHeartbeat > tcp->m_HeartbeatDelay)
			tcp->DropClient(); // Try it without semaphore, and fix it if needed xd

		vTaskDelay(tcp->m_HeartbeatDelay);
	}
}

int CTcp::get_socket_error_code(int socket)
{
	int result;
	u32_t optlen = sizeof(int);
	int err = getsockopt(socket, SOL_SOCKET, SO_ERROR, &result, &optlen);
	if (err == -1) {
		//ESP_LOGE(TAG, "getsockopt failed:%s", strerror(err));
		return -1;
	}
	return result;
}

int CTcp::show_socket_error_reason(const char *str, int socket)
{
	int err = get_socket_error_code(socket);

	if (err != 0) {
		//ESP_LOGW(TAG, "%s socket error %d %s", str, err, strerror(err));
		uart << str << " socket error " << err << " " << strerror(err) << endl;
	}

	return err;
}
