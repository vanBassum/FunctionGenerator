/*
 * FunctionGenerator.h
 *
 *  Created on: 4 Nov 2020
 *      Author: Bas
 */

#ifndef MAIN_FUNCTIONGENERATOR_H_
#define MAIN_FUNCTIONGENERATOR_H_

#include "AD9850.h"
#include "Display/TFT.h"
#include "Keypad.h"
#include "driver/gpio.h"
#include "../components/cpp_lib/FreeRTOS.h"
#include <string>
#include "Encoder.h"
#include <math.h>



class FunctionGenerator
{

	enum class Events : uint32_t	//These are flags!
	{
		StateChanged		= (1<<0),
		UpdateScreen 		= (1<<1),
		FreqChanged 		= (1<<2),

	};

	enum class State : uint8_t
	{
		Initializing		= 1,
		Running				= 2,
	};

	Task<NotifyEventGroup> *task = 0;
	AD9850 *ad9850 = 0;
	Keypad *keypad = 0;
	TFT tft;
	Font *font = 0;
	Encoder *encoder;

	std::string input;

	float frequency = 0;



	int ToHumanReadable(char* buf, int size, float value)
	{
	    const char plusSign = ' ';              //Use \0 to not show this char and not use any space
	    const char prefix[] = "fpnum kMGT";     //Use \0 to not show this char and not use any space
	    float absValue = 0;
	    bool isNegative = value < 0;

	    int digits = size - 4;                  //terminator + dot + sign + prefix = 4 characters.

	    if (digits <= 0)
	    {
	        if (size >= 1)
	        {
	            memset(buf, '-', size);         //Char to display when size is invalid.
	            buf[size - 1] = '\0';
	        }
	        return -1;
	    }

	    if (isNegative)
	        absValue = -value * pow(10, 15);    //Will this be optimized by compiler???
	    else
	        absValue = value * pow(10, 15);

	    int i = 0;

	    for(i=0; (i<sizeof(prefix)-2) && (absValue>=1000); i++)
	        absValue /= 1000;

	    int ditigsBeforeDot = log10(absValue)+1;
	    int ditigsAfterDot = digits - ditigsBeforeDot;

	    int wrt = 0;

	    if (isNegative)
	        buf[wrt++] = '-';	                //Add minus sign.
	    else if(plusSign != '\0')
	        buf[wrt++] = plusSign;              //Add plus sign.

	    if (ditigsAfterDot == 0)                //Dont have to show the dot, so add space
	        buf[wrt++] = ' ';

	    wrt += snprintf(&buf[wrt], size - wrt, "%.*f", ditigsAfterDot, absValue);

	    if(prefix[i] != '\0')
	        buf[wrt++] = prefix[i];
	    buf[wrt++] = '\0';
	    return wrt;
	}



	void Work()
	{
		tft.Init();
		tft.FillScreen(Color(0, 0, 0));

		State prevState = State::Initializing;
		State actState 	= State::Initializing;
		State nextState = State::Initializing;

		task->notify.Set((uint32_t) Events::StateChanged);

		tft.SetFontFill(Color(0,0,0));

		while(true)
		{
			uint32_t events = 0;
			task->notify.Wait(&events, 10);

			switch(actState)
			{
			case State::Initializing:
				if(ISSET(events, Events::StateChanged))
				{
					//On entry.
					tft.DrawString(font, 0, 16, "fSet", Color(255, 0, 0));
					tft.DrawString(font, 0, 32, "fAct", Color(255, 0, 0));
					task->notify.SetFromISR((uint32_t) Events::UpdateScreen);
				}

				nextState = State::Running;
				break;

			case State::Running:

				if(ISSET(events, Events::FreqChanged))
				{
					ad9850->SetFrequency(frequency);
				}

				if(ISSET(events, Events::UpdateScreen))
				{
					char buf[8];
					//tft.DrawFillRect(32, 0, 128, 32, Color(0, 0, 0));

					tft.DrawString(font, 32 + 8, 16, input + "Hz", Color(255, 0, 0));
					ToHumanReadable(buf, sizeof(buf), frequency);
					tft.DrawString(font, 32 + 8, 32, std::string(buf) + "Hz", Color(255, 0, 0));
				}

				break;
			}

			if(actState != nextState)
			{
				prevState = actState;
				actState = nextState;
				task->notify.Set((uint32_t) Events::StateChanged);	//Setting this will skip the 1sec delay and let the new state know its its first cycle.
			}
		}
	}

	float a = (log(50000.0) - log(10.0)) / 512;
	float b = log(10.0);

	float EncToFreq(float x)
	{
		return exp( a * x + b );
	}

	float FreqToEnc(float x)
	{
		return  (log(x) - b) / a;
	}


	void KeyPressed(char key)
	{
		if(key == '#')
		{
			frequency = std::stof(input);
			encoder->SetCount((int32_t)FreqToEnc(frequency));
			input = "";
			task->notify.SetFromISR((uint32_t) Events::FreqChanged);
		}
		else
			input += key;

		task->notify.SetFromISR((uint32_t) Events::UpdateScreen);
	}

	void CountAbsChanged(int32_t count)
	{
		frequency = EncToFreq(count);
		task->notify.SetFromISR((uint32_t) Events::FreqChanged);
		task->notify.SetFromISR((uint32_t) Events::UpdateScreen);
	}

public:

	FunctionGenerator()
	{
		ad9850 = new AD9850(GPIO_NUM_17,
						GPIO_NUM_16,
						GPIO_NUM_2,
						GPIO_NUM_32);

		keypad = new Keypad(GPIO_NUM_13,
						GPIO_NUM_12,
						GPIO_NUM_14,
						GPIO_NUM_27,
						GPIO_NUM_26,
						GPIO_NUM_25,
						GPIO_NUM_33);

		font = new Font(ILGH16XB);

		encoder = new Encoder();



		task = new Task<NotifyEventGroup>("funcgen", 7, 1024 * 4, this, &FunctionGenerator::Work);
		task->Run();

		keypad->ButtonPressed.Bind(this, &FunctionGenerator::KeyPressed);
		//keypad.ButtonReleased.Bind(this, &FunctionGenerator::KeyReleased);
		encoder->CountAbsChanged.Bind(this, &FunctionGenerator::CountAbsChanged);

	}


};


#endif /* MAIN_FUNCTIONGENERATOR_H_ */
