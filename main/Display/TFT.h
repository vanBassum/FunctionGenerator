/*
 * TFT.h
 *
 *  Created on: 2 Nov 2020
 *      Author: Bas
 */

#ifndef MAIN_DISPLAY_TFT_H_
#define MAIN_DISPLAY_TFT_H_

extern "C"
{
	#include "ili9340.h"
}

#include "Color.h"

class TFT
{
	TFT_t dev;

public:
	enum class Models
	{
		ILI9225 	= 0x9225,
		ILI9225G 	= 0x9226,
		ILI9340 	= 0x9340,
		ILI9341		= 0x9341,
		ST7735		= 0x7735,
		ST7796		= 0x7796,
	};

	int16_t GPIO_CS 	= 5;
	int16_t GPIO_DC 	= 22;
	int16_t GPIO_RESET 	= 4;
	int16_t GPIO_BL 	= 21;
	Models Model 		= Models::ST7735;
	uint16_t Width		= 128;
	uint16_t Height		= 128;
	uint16_t Offsetx	= 2;
	uint16_t Offsety	= 1;


	void Init()
	{
		spi_master_init(&dev, GPIO_CS, GPIO_DC, GPIO_RESET, GPIO_BL);
		lcdInit(&dev, 0x7735, Width, Height, Offsetx, Offsety);
	}







	void DrawPixel(uint16_t x, uint16_t y, Color color)
	{
		lcdDrawPixel( & dev, x, y, color.Get_RGB565());
	}

	void DrawFillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color)
	{
		lcdDrawFillRect( & dev, x1, y1, x2, y2, color.Get_RGB565());
	}

	void FillScreen(Color color)
	{
		lcdFillScreen( & dev, color.Get_RGB565());
	}

	void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color)
	{
		lcdDrawLine( & dev, x1, y1, x2, y2, color.Get_RGB565());
	}

	void DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color)
	{
		lcdDrawRect( & dev, x1, y1, x2, y2, color.Get_RGB565());
	}

	int DrawString(FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, Color color)
	{
		return lcdDrawString( & dev, fx, x, y, ascii, color.Get_RGB565());
	}


	/*



	void WriteRegisterWord(uint16_t addr, uint16_t data)
	{
		lcdWriteRegisterWord( & dev, addr, data);
	}

	void WriteRegisterByte(uint8_t addr, uint16_t data)
	{
		lcdWriteRegisterByte( & dev,  addr, data);
	}



	void DrawMultiPixels(uint16_t x, uint16_t y, uint16_t size, uint16_t * colors)
	{
		lcdDrawMultiPixels( & dev, x, y, size, colors);
	}



	void DisplayOff()
	{
		lcdDisplayOff( & dev);
	}

	void DisplayOn()
	{
		lcdDisplayOn( & dev);
	}

	void InversionOff()
	{
		lcdInversionOff( & dev);
	}

	void InversionOn()
	{
		lcdInversionOn( & dev);
	}

	void BGRFilter()
	{
		lcdBGRFilter( & dev);
	}



	void DrawRectAngle(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color)
	{
		lcdDrawRectAngle( & dev, xc, yc, w, h, angle, color);
	}

	void DrawTriangle(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color)
	{
		lcdDrawTriangle( & dev, xc, yc, w, h, angle, color);
	}

	void DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
	{
		lcdDrawCircle( & dev, x0, y0, r, color);
	}

	void DrawFillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
	{
		lcdDrawFillCircle( & dev, x0, y0, r, color);
	}

	void DrawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color)
	{
		lcdDrawRoundRect( & dev, x1, y1, x2, y2, r, color);
	}

	void DrawArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color)
	{
		lcdDrawArrow( & dev, x0, y0, x1, y1, w, color);
	}

	void DrawFillArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color)
	{
		lcdDrawFillArrow( & dev, x0, y0, x1, y1, w, color);
	}

	int DrawChar(FontxFile *fx, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color)
	{
		return lcdDrawChar( & dev, fx, x, y,  ascii, color);
	}

	int DrawString(FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color)
	{
		return lcdDrawString( & dev, fx, x, y, ascii, color);
	}

	void SetFontDirection(uint16_t dir)
	{
		lcdSetFontDirection( & dev, dir);
	}

	void SetFontFill(uint16_t color)
	{
		lcdSetFontFill( & dev, color);
	}

	void UnsetFontFill()
	{
		lcdUnsetFontFill( & dev);
	}

	void SetFontUnderLine(uint16_t color)
	{
		lcdSetFontUnderLine( & dev, color);
	}

	void UnsetFontUnderLine()
	{
		lcdUnsetFontUnderLine( & dev);
	}

	void BacklightOff()
	{
		lcdBacklightOff( & dev);
	}

	void BacklightOn()
	{
		lcdBacklightOn( & dev);
	}

	void SetScrollArea(uint16_t tfa, uint16_t vsa, uint16_t bfa)
	{
		lcdSetScrollArea( & dev, tfa, vsa, bfa);
	}

	void ResetScrollArea(uint16_t vsa)
	{
		lcdResetScrollArea( & dev, vsa);
	}

	void Scroll(uint16_t vsp)
	{
		lcdScroll( & dev, vsp);
	}

*/

};


#endif /* MAIN_DISPLAY_TFT_H_ */
