/*
 * Commands.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_COMMANDS_H_
#define MAIN_COMMANDS_H_

#include "../components/CPP_LIB/JBVProtocol.h"
#include "AD9850.h"

extern AD9850 ad9850;
typedef void (*Command)(JBVClient*, Frame*);



void SendACK(JBVClient *client, Frame *rx)
{
	Frame frame = Frame::PrepareReply(rx, 0, Commands::ReplyACK, FrameOPT::None);
	client->SendFrame(&frame);
}

void SendNACK(JBVClient *client, Frame *rx)
{
	Frame frame = Frame::PrepareReply(rx, 0, Commands::ReplyNACK, FrameOPT::None);
	client->SendFrame(&frame);
}


void SetFreqLed(JBVClient *client, Frame *rx)
{
	if(rx->DataLength == 8)
	{
		double *freq = (double*)rx->Data;
		ad9850.SetFrequency(*freq);
		SendACK(client, rx);
	}
	else
	{
		SendNACK(client, rx);
	}
}




void SwitchLed(JBVClient *client, Frame *rx)
{
	if(rx->DataLength == 1)
	{
		gpio_set_level(GPIO_NUM_2, rx->Data[0]);
		SendACK(client, rx);
	}
	else
	{
		SendNACK(client, rx);
	}
}


Command GetCMD(uint32_t i)
{
	switch(i)
	{
	case 1: return SwitchLed;
	case 2: return SetFreqLed;
	default: return 0;
	}

}




void HandleFrame(JBVClient *client, Frame *rx)
{
	Command cmd = GetCMD(rx->CommandID);
	if(cmd != 0)
	{
		cmd(client, rx);
	}
	else
	{
		Frame tx = Frame::PrepareReply(rx, 0, Commands::ReplyCMDINV);
		client->SendFrame(&tx);
		ESP_LOGE("Commands", "Command not defined '%d'", rx->CommandID);
	}

}



#endif /* MAIN_COMMANDS_H_ */
