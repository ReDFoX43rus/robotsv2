#include "wifi.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "string.h"
#include "nvs_flash.h"

#include "freertos/event_groups.h"

static EventGroupHandle_t event_group;
#define WIFI_CONNECTED_BIT BIT0

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
	    case SYSTEM_EVENT_STA_START:
	        esp_wifi_connect();
	        break;
	    case SYSTEM_EVENT_STA_DISCONNECTED:
	        esp_wifi_connect();
	        xEventGroupClearBits(event_group, WIFI_CONNECTED_BIT);
	        break;
	    case SYSTEM_EVENT_STA_CONNECTED:
	        break;
	    case SYSTEM_EVENT_STA_GOT_IP:
	        printf("wifi: got ip:%s\n",
	                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
	        xEventGroupSetBits(event_group, WIFI_CONNECTED_BIT);
	        break;
	    case SYSTEM_EVENT_AP_STACONNECTED:
	        printf("wifi: station:"MACSTR" join,AID=%d\n",
	                 MAC2STR(event->event_info.sta_connected.mac),
	                 event->event_info.sta_connected.aid);
	        xEventGroupSetBits(event_group, WIFI_CONNECTED_BIT);
	        break;
	    case SYSTEM_EVENT_AP_STADISCONNECTED:
	        printf("wifi: station:"MACSTR"leave,AID=%d\n",
	                 MAC2STR(event->event_info.sta_disconnected.mac),
	                 event->event_info.sta_disconnected.aid);
	        xEventGroupClearBits(event_group, WIFI_CONNECTED_BIT);
	        break;
	    default:
	        break;
    }
    return ESP_OK;
}

int wifi_init_sta(const char *ssid, const char *pwd){
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
		// OTA app partition table has a smaller NVS partition size than the non-OTA
		// partition table. This size mismatch may cause NVS initialization to fail.
		// If this happens, we erase NVS partition and initialize NVS again.
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	if(err != ESP_OK)
		return -5;

	event_group = xEventGroupCreate();

	tcpip_adapter_init();
	err = esp_event_loop_init(event_handler, NULL);
	if(err != ESP_OK)
		return -6;

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	if(esp_wifi_init(&cfg) != ESP_OK)
		return -4;

	wifi_config_t wifi_config;

	memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
	memcpy(wifi_config.sta.password, pwd, strlen(pwd));

	wifi_config.sta.ssid[strlen(ssid)] = '\0';
	wifi_config.sta.password[strlen(pwd)] = '\0';

	printf("ssid: %s, pwd: %s\n", wifi_config.sta.ssid, wifi_config.sta.password);

	if(esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK)
		return -1;

	if(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) != ESP_OK)
		return -2;

	if(esp_wifi_start() != ESP_OK)
		return -3;

	return 0;
}
