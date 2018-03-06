#include "wifi.h"

#include "string.h"
#include "nvs_flash.h"
#include "uart.h"

#define MAX_SCAN_AP_RECORDS 32

EventGroupHandle_t CWifi::m_EventGroup = xEventGroupCreate();

esp_err_t CWifi::event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id) {
		case SYSTEM_EVENT_STA_START:
			//esp_wifi_connect();
			uart << "STA start" << endl;
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			uart << "STA disconnected" << endl;
			esp_wifi_connect();
			xEventGroupClearBits(CWifi::m_EventGroup, WIFI_CONNECTED_BIT);
			break;
		case SYSTEM_EVENT_STA_CONNECTED:
			uart << "STA connected" << endl;
			break;
		case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
			uart << "STA wps ok" << endl;
			break;
		case SYSTEM_EVENT_STA_WPS_ER_FAILED:
			uart << "STA wps failed" << endl;
			break;
		case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
			uart << "STA wps timeouted" << endl;
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			printf("wifi: got ip:%s\n",
					 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			xEventGroupSetBits(CWifi::m_EventGroup, WIFI_CONNECTED_BIT);
			break;
		case SYSTEM_EVENT_AP_STACONNECTED:
			printf("wifi: station:"MACSTR" join,AID=%d\n",
					 MAC2STR(event->event_info.sta_connected.mac),
					 event->event_info.sta_connected.aid);
			xEventGroupSetBits(CWifi::m_EventGroup, WIFI_CONNECTED_BIT);
			break;
		case SYSTEM_EVENT_AP_STADISCONNECTED:
			printf("wifi: station:"MACSTR"leave,AID=%d\n",
					 MAC2STR(event->event_info.sta_disconnected.mac),
					 event->event_info.sta_disconnected.aid);
			xEventGroupClearBits(CWifi::m_EventGroup, WIFI_CONNECTED_BIT);
			break;
		default:
			break;
	}
	return ESP_OK;
}

CWifi::CWifi(){
	m_ScanRecords = NULL;
}

CWifi::~CWifi(){
	if(m_Inited)
		esp_wifi_deinit();

	if(m_ScanRecords)
		delete[] m_ScanRecords;
}

int CWifi::Init(){
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	if(err != ESP_OK)
		return -1;

	tcpip_adapter_init();
	err = esp_event_loop_init(event_handler, NULL);
	if(err != ESP_OK)
		return -2;

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	if(esp_wifi_init(&cfg) != ESP_OK)
		return -3;

	wifi_config_t wifi_config;
	wifi_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
	wifi_config.sta.channel = 0;

	if(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) != ESP_OK)
		return -6;

	if(esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK)
		return -4;

	if(esp_wifi_start() != ESP_OK)
		return -5;

	m_Inited = true;
	return 0;
}

int CWifi::Search(uint8_t channel){
	if(m_ScanRecords)
		return -10;

	wifi_scan_config_t scfg;
	scfg.scan_type = WIFI_SCAN_TYPE_PASSIVE;
	scfg.ssid = NULL;
	scfg.bssid = NULL;
	scfg.channel = channel;
	scfg.show_hidden = true;

	if(esp_wifi_scan_start(&scfg, true) != ESP_OK)
		return -1;

	if(esp_wifi_scan_get_ap_num(&m_CurrentScanRecords) != ESP_OK)
		return -2;

	m_ScanRecords = new wifi_ap_record_t[m_CurrentScanRecords];
	if(!m_ScanRecords)
		return -11;

	if(esp_wifi_scan_get_ap_records(&m_CurrentScanRecords, m_ScanRecords) != ESP_OK)
		return -3;

	return 0;
}

void CWifi::ListScanRecords(){
	if(!m_ScanRecords)
		return;

	for(int i = 0; i < m_CurrentScanRecords; i++){
		uart << "Found " << (char*)m_ScanRecords[i].ssid
		<< " on channel: " << m_ScanRecords[i].primary << endl;
	}

	delete[] m_ScanRecords;
	m_ScanRecords = NULL;
}

int CWifi::Connect(char *ssid, char *pwd){
	wifi_config_t wifi_config;

	strcpy((char*)wifi_config.sta.ssid, ssid);
	strcpy((char*)wifi_config.sta.password, pwd);

	esp_wifi_disconnect();

	if(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) != ESP_OK)
		return -2;

	if(esp_wifi_connect() != ESP_OK)
		return -3;

	return 0;
}

// int wifi_init_sta(const char *ssid, const char *pwd){
// 	esp_err_t err = nvs_flash_init();
// 	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
// 		// OTA app partition table has a smaller NVS partition size than the non-OTA
// 		// partition table. This size mismatch may cause NVS initialization to fail.
// 		// If this happens, we erase NVS partition and initialize NVS again.
// 		ESP_ERROR_CHECK(nvs_flash_erase());
// 		err = nvs_flash_init();
// 	}
// 	if(err != ESP_OK)
// 		return -5;
//
// 	event_group = xEventGroupCreate();
//
// 	tcpip_adapter_init();
// 	err = esp_event_loop_init(event_handler, NULL);
// 	if(err != ESP_OK)
// 		return -6;
//
// 	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//
// 	if(esp_wifi_init(&cfg) != ESP_OK)
// 		return -4;
//
// 	wifi_config_t wifi_config;
//
// 	memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
// 	memcpy(wifi_config.sta.password, pwd, strlen(pwd));
//
// 	wifi_config.sta.ssid[strlen(ssid)] = '\0';
// 	wifi_config.sta.password[strlen(pwd)] = '\0';
//
// 	printf("ssid: %s, pwd: %s\n", wifi_config.sta.ssid, wifi_config.sta.password);
//
// 	if(esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK)
// 		return -1;
//
// 	if(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) != ESP_OK)
// 		return -2;
//
// 	if(esp_wifi_start() != ESP_OK)
// 		return -3;
//
// 	return 0;
// }
