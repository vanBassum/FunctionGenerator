/*
 * Commands.h
 *
 *  Created on: 23 Oct 2020
 *      Author: Bas
 */

#ifndef MAIN_COMMANDS_H_
#define MAIN_COMMANDS_H_

#include "../components/jbvprotocol/jbvprotocol.h"

extern TFT tft;

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

void FillScreen(JBVClient *client, Frame *rx)
{
	if(rx->DataLength == 3)
	{
		tft.FillScreen(Color(rx->Data[0], rx->Data[1], rx->Data[2]));
		SendACK(client, rx);
	}
	else
	{
		SendNACK(client, rx);
	}
}

void DrawLine(JBVClient *client, Frame *rx)
{
	if(rx->DataLength == 8 + 3)
	{

		uint16_t x1 = rx->Data[0] | rx->Data[1]<<8;
		uint16_t y1 = rx->Data[2] | rx->Data[3]<<8;
		uint16_t x2 = rx->Data[4] | rx->Data[5]<<8;
		uint16_t y2 = rx->Data[6] | rx->Data[7]<<8;

		Color c = Color(rx->Data[8], rx->Data[9], rx->Data[10]);

		//ESP_LOGI("cmd", "%d %d %d %d ", x1, y1, x2, y2);

		tft.DrawLine(x1, y1, x2, y2, c);
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
	case 2: return FillScreen;
	case 3: return DrawLine;
	default: return 0;
	}

}


void HandleFrame(JBVClient *client, Frame *rx)
{
	//ESP_LOGI("Commands", "CMD Recieved %d with datalength %d", rx->CommandID, rx->DataLength);

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
