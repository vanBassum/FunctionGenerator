#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_sntp.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string.h>
#include "lwip/apps/sntp.h"
#include "lwip/apps/sntp_opts.h"
#include "../components/jbvprotocol/jbvclient.h"
#include "../components/tcpip/tcpconnection.h"
#include "../components/tft/tft.h"

#include "Commands.h"

JBVClient client(SoftwareID::FunctionGenerator);
TFT tft = TFT::Get_ILI9341();


extern "C" {
   void app_main();
}



esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id)
	{
	case SYSTEM_EVENT_STA_GOT_IP:
		break;
	default:
		break;
	}

    return ESP_OK;
}




void app_main(void)
{
    nvs_flash_init();

    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    wifi_config_t sta_config = {};
    memcpy(sta_config.sta.ssid, "vanBassum", 10);
    memcpy(sta_config.sta.password, "pedaalemmerzak", 15);
    sta_config.sta.bssid_set = false;
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    setenv("TZ", "UTC-1UTC,M3.5.0,M10.5.0/3", 1);
	tzset();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();

	TCPConnection con;
	con.Connect("192.168.11.50", 32770, true);
	client.SetConnection(&con);
	client.HandleFrame.Bind(HandleFrame);




	tft.Init();
	tft.FillScreen(Color(0, 0, 0));
	tft.DrawLine(110, 150, 130, 170, Color(255,0,0));
	tft.DrawLine(130, 150, 110, 170, Color(255,0,0));
	tft.DrawLine(0, 0, 240, 320, Color(255,0,0));

	float fps = 1;

	int delayMs = 1000 / fps;

	while(1)
	{

		vTaskDelay(delayMs / portTICK_PERIOD_MS);
	}




}






