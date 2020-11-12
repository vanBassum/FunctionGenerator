/*
 * Encoder.h
 *
 *  Created on: 10 Nov 2020
 *      Author: Bas
 */

#ifndef MAIN_ENCODER_H_
#define MAIN_ENCODER_H_


#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "driver/pcnt.h"

class Encoder
{
public:
	Event<int32_t> CountAbsChanged;
	Event<int16_t> CountRelChanged;

	int32_t GetCount()
	{
		return count;
	}

	void SetCount(int32_t val)
	{
		count = val;
	}


private:
	Timer checkCountTimer = Timer("quadtimer", 100, true);
	int16_t prevCount = 0;
	int32_t count = 0;

public:

	void checkCountTimer_Elapsed()
	{
		int16_t val;
		pcnt_get_counter_value(PCNT_UNIT_0, &val);
		pcnt_counter_clear(PCNT_UNIT_0);
		if(prevCount != val)
		{
			prevCount = val;
			count += val;
			CountAbsChanged(count);
			CountRelChanged(val);
		}
	}


	Encoder()
	{


		pcnt_config_t dev_config;
        dev_config.pulse_gpio_num = GPIO_NUM_34;
        dev_config.ctrl_gpio_num = GPIO_NUM_35;
        dev_config.channel = PCNT_CHANNEL_0;
        dev_config.unit = PCNT_UNIT_0;
        dev_config.pos_mode = PCNT_COUNT_DEC;
        dev_config.neg_mode = PCNT_COUNT_INC;
        dev_config.lctrl_mode = PCNT_MODE_REVERSE;
        dev_config.hctrl_mode = PCNT_MODE_KEEP;
        dev_config.counter_h_lim = 0;
        dev_config.counter_l_lim = 0;


		if(pcnt_unit_config(&dev_config) != ESP_OK )
			ESP_LOGE("Encoder", "Config error");

		pcnt_counter_clear(PCNT_UNIT_0);

		checkCountTimer.OnTimerElapsed.Bind(this, &Encoder::checkCountTimer_Elapsed);
		checkCountTimer.SetPeriod(25);
		checkCountTimer.Start();

	}






};



#endif /* MAIN_ENCODER_H_ */
