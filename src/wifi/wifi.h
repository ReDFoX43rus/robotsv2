#ifndef WIFI_WIFI_H
#define WIFI_WIFI_H

#include "esp_wifi.h"
#include "unistd.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "lwip/sockets.h"

//#include "iobase/wifiio/wifiio.h"

#define TAG "tcp_perf:"

//mask always 255.255.255.0
//IP:192.168.1.1:3000

const int CLIENT_CONNECTED_BIT = BIT0;
const int CLIENT_DISCONNECTED_BIT = BIT1;
const int AP_STARTED_BIT = BIT2;
const int WAIT_FOR_STOP_BIT = BIT3;
const int DOWNLOADED_BIT = BIT4;


class CWifi {
public:
	CWifi(/*CWifiIO *wio*/);
	~CWifi();
	void wifi_stop();
	void wifi_start();
	void acces_point_update();

private:
	bool wifi_started;
	void wifi_init_softap();
	static void tcp_server_init(void *arg);
	static void startTaskInit(void*);
	static void start_dhcp_server();

	int response_to_client(int client_socket);
	static int compare_clients(struct sockaddr *first, struct sockaddr *second) {return strcmp(first->sa_data, second->sa_data);}

	//CWifiIO *m_Wio;
};

static EventGroupHandle_t wifi_event_group;
void print_sta_info(void *pvParam);
static esp_err_t event_handler(void *ctx, system_event_t *event);

#endif
