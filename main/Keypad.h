/*
 * Keypad.h
 *
 *  Created on: 3 Nov 2020
 *      Author: Bas
 */

#ifndef MAIN_KEYPAD_H_
#define MAIN_KEYPAD_H_

#include "../components/cpp_lib/FreeRTOS.h"
#include "driver/gpio.h"
#include "../components/cpp_lib/Misc/Event.h"

class Keypad
{
public:
	Event<char> ButtonPressed;
	Event<char> ButtonReleased;
private:
	Timer buttonsTimer = Timer("btntmr", 100, true);

	gpio_num_t pinA;
	gpio_num_t pinB;
	gpio_num_t pinC;
	gpio_num_t pinD;
	gpio_num_t pin1;
	gpio_num_t pin2;
	gpio_num_t pin3;

	uint8_t cnt = 0;
	uint16_t btn = 0;
	uint16_t pbtn = 0;

private:

	void btnTimer()
	{
		gpio_set_level(pin1, cnt != 0);
		gpio_set_level(pin2, cnt != 1);
		gpio_set_level(pin3, cnt != 2);

		ets_delay_us(1);

		bool a = !gpio_get_level(pinA);
		bool b = !gpio_get_level(pinB);
		bool c = !gpio_get_level(pinC);
		bool d = !gpio_get_level(pinD);

		btn = btn & ~(0xF << cnt * 4);
		btn |= a<<(0 + cnt*4);
		btn |= b<<(1 + cnt*4);
		btn |= c<<(2 + cnt*4);
		btn |= d<<(3 + cnt*4);

		if(btn != pbtn)
		{
			uint16_t changes = btn ^ pbtn;		//Detect changed bits
			uint16_t pressed = changes & btn;	//These where pressed just now
			uint16_t released = changes & ~btn;	//These where released just now

			if(pressed & (1<< 0)) ButtonPressed('#');
			if(pressed & (1<< 1)) ButtonPressed('9');
			if(pressed & (1<< 2)) ButtonPressed('6');
			if(pressed & (1<< 3)) ButtonPressed('3');
			if(pressed & (1<< 4)) ButtonPressed('0');
			if(pressed & (1<< 5)) ButtonPressed('8');
			if(pressed & (1<< 6)) ButtonPressed('5');
			if(pressed & (1<< 7)) ButtonPressed('2');
			if(pressed & (1<< 8)) ButtonPressed('.');
			if(pressed & (1<< 9)) ButtonPressed('7');
			if(pressed & (1<<10)) ButtonPressed('4');
			if(pressed & (1<<11)) ButtonPressed('1');

			if(released & (1<< 0)) ButtonReleased('#');
			if(released & (1<< 1)) ButtonReleased('9');
			if(released & (1<< 2)) ButtonReleased('6');
			if(released & (1<< 3)) ButtonReleased('3');
			if(released & (1<< 4)) ButtonReleased('0');
			if(released & (1<< 5)) ButtonReleased('8');
			if(released & (1<< 6)) ButtonReleased('5');
			if(released & (1<< 7)) ButtonReleased('2');
			if(released & (1<< 8)) ButtonReleased('.');
			if(released & (1<< 9)) ButtonReleased('7');
			if(released & (1<<10)) ButtonReleased('4');
			if(released & (1<<11)) ButtonReleased('1');


			pbtn = btn;
		}

		cnt++;
		if(cnt >= 3)
			cnt = 0;
	}


public:

	Keypad(	gpio_num_t pinA,
			gpio_num_t pinB,
			gpio_num_t pinC,
			gpio_num_t pinD,
			gpio_num_t pin1,
			gpio_num_t pin2,
			gpio_num_t pin3)
	{
		this->pinA = pinA;
		this->pinB = pinB;
		this->pinC = pinC;
		this->pinD = pinD;
		this->pin1 = pin1;
		this->pin2 = pin2;
		this->pin3 = pin3;

		gpio_reset_pin(pinA);
		gpio_reset_pin(pinB);
		gpio_reset_pin(pinC);
		gpio_reset_pin(pinD);
		gpio_reset_pin(pin1);
		gpio_reset_pin(pin2);
		gpio_reset_pin(pin3);

		gpio_set_direction(pinA, GPIO_MODE_INPUT);
		gpio_set_direction(pinB, GPIO_MODE_INPUT);
		gpio_set_direction(pinC, GPIO_MODE_INPUT);
		gpio_set_direction(pinD, GPIO_MODE_INPUT);
		gpio_set_direction(pin1, GPIO_MODE_OUTPUT);
		gpio_set_direction(pin2, GPIO_MODE_OUTPUT);
		gpio_set_direction(pin3, GPIO_MODE_OUTPUT);

		buttonsTimer.OnTimerElapsed.Bind(this, &Keypad::btnTimer);
		buttonsTimer.SetPeriod(25);
		buttonsTimer.Start();
	}
};


#endif /* MAIN_KEYPAD_H_ */
