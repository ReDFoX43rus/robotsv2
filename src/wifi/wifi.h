#ifndef WIFI_WIFI_H
#define WIFI_WIFI_H

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"

class CWifi {

public:
	static EventGroupHandle_t m_EventGroup;
	static esp_err_t event_handler(void *ctx, system_event_t *event);

	CWifi();
	~CWifi();

	int Init();
	int Destroy();
	int Search(uint8_t channel);
	void ListScanRecords();
	int Connect(char *ssid, char *pwd);

private:
	bool m_Inited;
	wifi_ap_record_t *m_ScanRecords;
	uint16_t m_CurrentScanRecords;
	wifi_config_t wifi_config;

	enum{
		WIFI_CONNECTED_BIT=BIT0
	};
};
#endif /* end of include guard: WIFI_WIFI_H */
