/*
 * AD9850.h
 *
 *  Created on: 1 Nov 2020
 *      Author: Bas
 */

#ifndef MAIN_AD9850_H_
#define MAIN_AD9850_H_

#include "driver/gpio.h"
#include <stdint.h>
#include "esp_log.h"

#define AD9850_PULSEDURATION	5	//µS

class AD9850
{
	gpio_num_t wclk;
	gpio_num_t fqud;
	gpio_num_t data;
	gpio_num_t reset;


	inline void Pulse(gpio_num_t pin, bool high)
	{
		gpio_set_level(pin, high);
		ets_delay_us(AD9850_PULSEDURATION);
		gpio_set_level(pin, !high);
		ets_delay_us(AD9850_PULSEDURATION);
	}

	inline void SendByte(uint8_t byte)
	{
		for(int i=0; i<8; i++, byte>>=1)
		{
			gpio_set_level(data, byte & 0x01);
			Pulse(wclk, 1);
		}
	}

public:

	AD9850(	gpio_num_t pwclk,
			gpio_num_t pfqud,
			gpio_num_t pdata,
			gpio_num_t preset)
	{
		ESP_LOGI("AD9850", "Init");

		wclk  = pwclk ;
		data  = pdata ;
		fqud  = pfqud ;
		reset = preset;

		gpio_set_direction(wclk, GPIO_MODE_OUTPUT);
		gpio_set_direction(data, GPIO_MODE_OUTPUT);
		gpio_set_direction(fqud, GPIO_MODE_OUTPUT);
		gpio_set_direction(reset, GPIO_MODE_OUTPUT);

		vTaskSuspendAll ();
		Pulse(reset, 1);
		Pulse(wclk, 1);
		Pulse(fqud, 1);
		xTaskResumeAll ();
	}





	void SetFrequency(double frequency)
	{
		uint32_t freq = frequency * 4294967295 / 125000000;

		//ESP_LOGI("AD9850", "Setfreq %f", frequency);

		vTaskSuspendAll ();
		for(int i=0; i<4; i++, freq>>=8)
			SendByte(freq & 0xFF);

		SendByte(0x00);	//Control byte
		Pulse(fqud, 1);
		xTaskResumeAll ();

	}

};





#endif /* MAIN_AD9850_H_ */
