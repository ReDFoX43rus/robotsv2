#include "tcp_srv.h"

#include <sys/socket.h>
#include "esp_log.h"
#include "string.h"
#include "uart.h"

CTcp::CTcp(uint16_t port){
	m_Port = port;
	m_ServerSocket = 0;
	m_ConnectSocket = 0;
	m_CurrentBuffSize = 0;
	m_BuffSem = xSemaphoreCreateMutex();
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

	while((len = recv(m_ConnectSocket, buff, TCPIO_RECV_BUFF_SIZE, 0)) > 0){

		if(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) == pdTRUE){
			to_write = m_CurrentBuffSize + len <= TCPIO_RECV_BUFF_SIZE ? len : TCPIO_RECV_BUFF_SIZE - m_CurrentBuffSize;
			memcpy(m_IOBuffer + m_CurrentBuffSize, buff, to_write);
			m_CurrentBuffSize += to_write;

			uart << "recv: ";
			uart.Write(buff, to_write);
			uart << endl;
		}
		xSemaphoreGive(m_BuffSem);
	}

	return len;
}

uint32_t CTcp::GetChar(){
	char ret;

_take_sem:
	while(xSemaphoreTake(m_BuffSem, TCPIO_SEM_WAIT_TIME) != pdTRUE)
		;

	if(!m_CurrentBuffSize){
		xSemaphoreGive(m_BuffSem);
		vTaskDelay(pdMS_TO_TICKS(350));
		goto _take_sem;
	}

	ret = m_IOBuffer[m_CurrentBuffSize--];
	xSemaphoreGive(m_BuffSem);

	return ret;
}

size_t CTcp::Write(const char *data, size_t size){
	if(!m_ConnectSocket)
		return 0;

	int res = send(m_ConnectSocket, data, size, 0);

	return res >= 0 ? res : 0;
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
