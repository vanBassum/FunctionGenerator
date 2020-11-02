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
#include "../components/CPP_LIB/FreeRTOS.h"
#include "../components/CPP_LIB/JBVProtocol.h"
#include "Commands.h"
#include "AD9850.h"
#include "Display/TFT.h"



TCPConnection con;
JBVClient client(SoftwareID::FunctionGenerator);

AD9850 ad9850(	GPIO_NUM_26,
				GPIO_NUM_33,
				GPIO_NUM_25,
				GPIO_NUM_32);

TFT tft;



extern "C" {
   void app_main();
}



esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id)
	{
	case SYSTEM_EVENT_STA_GOT_IP:
		con.WifiConnected();
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

	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0);


	con.Connect("192.168.11.2", 1000, true);
	client.SetConnection(&con);
	client.HandleFrame = new Callback<void, JBVClient*, Frame*>(HandleFrame);










	ad9850.SetFrequency(100);

	tft.Init();
	tft.FillScreen(Color(0, 0, 0));






    while(true)
    {
    	uint16_t size = 100 * 7;
    	uint8_t rand[size];
    	esp_fill_random(&rand, size);


    	tft.FillScreen(Color(0, 0, 0));


    	for(int i = 0; i<size; i+=7)
    	{
    		tft.DrawLine(rand[i]/2, rand[i+1]/2, rand[i+2]/2, rand[i+3]/2, Color(rand[i+4], rand[i+5], rand[i+6]));
    	}


    }
}






