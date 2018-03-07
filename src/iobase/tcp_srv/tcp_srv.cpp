#include "tcp_srv.h"

#include <sys/socket.h>
#include "esp_log.h"
#include "string.h"
#include "uart.h"
#include "cconsole/console.h"

CTcp::CTcp(uint16_t port){
	m_Port = port;
	m_ServerSocket = 0;
	m_ConnectSocket = 0;
	m_QueueFront = 0;
	m_QueueBack = 0;
	m_BuffSem = xSemaphoreCreateMutex();
	m_ConsoleTask = 0;
}

CTcp::~CTcp(){
	if(m_ConnectSocket)
		close(m_ConnectSocket);
	if(m_ServerSocket)
		close(m_ServerSocket);

	vSemaphoreDelete(m_BuffSem);
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

int CTcp::AcceptAndRecv(){
	struct sockaddr_in client_addr;
	socklen_t socklen = sizeof(client_addr);

	uart << "Tcp server is waiting for a client..." << endl;

	m_ConnectSocket = accept(m_ServerSocket, (struct sockaddr *)&client_addr, &socklen);

	if (m_ConnectSocket < 0) {
		show_socket_error_reason("accept_server", m_ConnectSocket);
		close(m_ServerSocket);
		return -1;
	}

	/*connection established，now can send/recv*/
	uart << "Client accepted" << endl;

	char buff[TCPIO_RECV_BUFF_SIZE];
	int len, to_write;
	int currentBuffSize = 0;

	while((len = recv(m_ConnectSocket, buff, TCPIO_RECV_BUFF_SIZE, 0)) > 0){

		while(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
			vTaskDelay(pdMS_TO_TICKS(50));

		if(m_QueueFront + len > TCPIO_MAX_BUFF_SIZE)
			BalanceQueue();

		to_write = m_QueueFront + len <= TCPIO_MAX_BUFF_SIZE ? len : TCPIO_MAX_BUFF_SIZE - m_QueueFront;
		memcpy(m_IOQueue + m_QueueFront, buff, to_write);
		m_QueueFront += to_write;

		if(len - to_write != 0){
			*this << "Lost " << len - to_write << " bytes" << endl;
		}

		currentBuffSize = m_QueueFront - m_QueueBack;

		// uart << "Back: " << m_QueueBack << " Front: " << m_QueueFront << endl;

		// uart << "recv: ";
		// uart.Write(buff, to_write);
		// uart << endl;

		xSemaphoreGive(m_BuffSem);
		if(currentBuffSize == TCPIO_MAX_BUFF_SIZE)
			vTaskDelay(pdMS_TO_TICKS(350));
	}

	close(m_ConnectSocket);

	return len;
}

void CTcp::AttachToConsole(void *arg){
	while(1)
		console.WaitForCmd(*(CTcp*)arg);
}

int CTcp::SetupConsole(){
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

_take_sem:
	while(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
		;

	if(m_QueueBack == m_QueueFront){
		xSemaphoreGive(m_BuffSem);
		vTaskDelay(pdMS_TO_TICKS(50));
		goto _take_sem;
	}

	ret = m_IOQueue[m_QueueBack++];

	// uart << "Returning char... Queue back: " << m_QueueBack << " queue front: " << m_QueueFront << endl;

	xSemaphoreGive(m_BuffSem);

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
_getBytes_take_sem:
	while(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
		;

	int buffSize = m_QueueFront - m_QueueBack;
	if(!buffSize){
		xSemaphoreGive(m_BuffSem);
		vTaskDelay(pdMS_TO_TICKS(50));
		goto _getBytes_take_sem;
	}

	if(size > buffSize)
		size = buffSize;

	memcpy(data, m_IOQueue + m_QueueBack, size);
	m_QueueBack += size;

	xSemaphoreGive(m_BuffSem);

	return size;
}

void CTcp::BalanceQueue(){
	for(int i = m_QueueBack; i <= m_QueueFront; i++)
		m_IOQueue[i - m_QueueBack] = m_IOQueue[i];

	m_QueueFront -= m_QueueBack;
	m_QueueBack = 0;
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
