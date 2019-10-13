//	MIT License
//
//	Copyright (c) 2017 Matej Artnak
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//
//
//
//-----------------------------------
//	ILI9341 GFX library for STM32
//-----------------------------------
//
//	Very simple GFX library built upon ILI9342_STM32_Driver library.
//	Adds basic shapes, image and font drawing capabilities to ILI9341
//
//	Library is written for STM32 HAL library and supports STM32CUBEMX. To use the library with Cube software
//	you need to tick the box that generates peripheral initialization code in their own respective .c and .h file
//
//
//-----------------------------------
//	How to use this library
//-----------------------------------
//
//	-If using MCUs other than STM32F7 you will have to change the #include "stm32f7xx_hal.h" in the ILI9341_GFX.h to your respective .h file
//
//	If using "ILI9341_STM32_Driver" then all other prequisites to use the library have allready been met
//	Simply include the library and it is ready to be used
//
//-----------------------------------

#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include <string.h>
#include "5x5_font.h"
#include "spi.h"

/*Draw hollow circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour) {
    int x = Radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (Radius << 1u);

    while (x >= y) {
        ILI9341_Draw_Pixel(X + x, Y + y, Colour);
        ILI9341_Draw_Pixel(X + y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - x, Y + y, Colour);
        ILI9341_Draw_Pixel(X - x, Y - y, Colour);
        ILI9341_Draw_Pixel(X - y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + x, Y - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-Radius * 2) + dx;
        }
    }
}

/*Draw filled circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius;
    int y = 0;
    int xChange = 1 - (Radius << 1u);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        ILI9341_Draw_Horizontal_Line(X - x, Y + y, 2 * x, Colour);
        ILI9341_Draw_Horizontal_Line(X - x, Y - y, 2 * x, Colour);
        ILI9341_Draw_Horizontal_Line(X - y, Y + x, 2 * y, Colour);
        ILI9341_Draw_Horizontal_Line(X - y, Y - x, 2 * y, Colour);
        y++;
        radiusError += yChange;
        yChange += 2;
        if ((radiusError * 2 + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

/*Draw a hollow rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;

    if((X1 - X0 )< 0) Negative_X = 1;

    if((Y1 - Y0) < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;		
	}
	else
	{
		X_length = X0 - X1;		
	}
	ILI9341_Draw_Horizontal_Line(X0, Y0, X_length, Colour);
	ILI9341_Draw_Horizontal_Line(X0, Y1, X_length, Colour);
	
	
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;		
	}
	ILI9341_Draw_Vertical_Line(X0, Y0, Y_length, Colour);
	ILI9341_Draw_Vertical_Line(X1, Y0, Y_length, Colour);
	
	if((X_length > 0)||(Y_length > 0)) 
	{
		ILI9341_Draw_Pixel(X1, Y1, Colour);
	}
	
}

/*Draw a filled rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;

	uint16_t X0_true = 0;
	uint16_t Y0_true = 0;
	
	if((X1 - X0) < 0) Negative_X = 1;

	if((Y1 - Y0) < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;
		X0_true = X0;
	}
	else
	{
		X_length = X0 - X1;
		X0_true = X1;
	}
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;
		Y0_true = Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;
		Y0_true = Y1;	
	}
	
	ILI9341_Draw_Rectangle(X0_true, Y0_true, X_length, Y_length, Colour);	
}


/*Draws an array of characters (fonts imported from fonts.h) at X,Y location with specified font colour, size and Background colour*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void
ILI9341_Draw_Text(const char *Text, uint8_t X, uint8_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour) {
    ILI9341_Draw_Text_Len(Text, strlen(Text), X, Y, Colour, Size, Background_Colour);
}


struct IMAGE_SEND_PARAMS {
    const uint8_t *ptr;
    uint32_t bytesLeft;
};

void ILI9341_Draw_Box_By_Chunks(uint16_t (*nextChunk)(void *paramBlock, const uint8_t **chunkAddress), void *paramBlock,
                                int x, int y, int w, int h) {
    ILI9341_Set_Address(x, y, x + w - 1, y + h - 1);
   ILI9341_dcOn();
   ILI9341_csOff();
    while (1) {
        const uint8_t *chunkAddress;
        int chunkSize = nextChunk(paramBlock, &chunkAddress);
        if (chunkSize == 0) break;
        ILI9341_transmit( (uint8_t *) chunkAddress, chunkSize);
    }
    ILI9341_csOn();
}

/**
 * Function to be called from ILI9341_Draw_Box_By_Chunks
 * @param paramBlock parameter block passed from outer function
 * @param chunkAddress OUT chunk address
 * @return length of the chunk, zero if out of data
 */
static uint16_t imageDataChunk(void *paramBlock, const unsigned char **chunkAddress) {
    struct IMAGE_SEND_PARAMS *params = paramBlock;
    uint32_t bytesToSend = params->bytesLeft >= 0xFFFEu ? 0xFFFEu : params->bytesLeft;
    params->bytesLeft -= bytesToSend;
    *chunkAddress = params->ptr;
    params->ptr += bytesToSend;
    return bytesToSend;
}

/*Draws a full screen picture from flash. Image converted from RGB .jpeg/other to C array using online converter*/
//USING CONVERTER: http://www.digole.com/tools/PicturetoC_Hex_converter.php
//65K colour (2Bytes / Pixel)
void ILI9341_Draw_Image(const char *Image_Array, uint8_t Orientation) {
    ILI9341_Set_Rotation(Orientation);
    struct IMAGE_SEND_PARAMS params = {.bytesLeft= 2 * ILI9341_SCREEN_HEIGHT * ILI9341_SCREEN_WIDTH,
            .ptr = (const uint8_t *) Image_Array};
    ILI9341_Draw_Box_By_Chunks(imageDataChunk, &params, 0, 0, LCD_WIDTH, LCD_HEIGHT);
}

struct TEXT_SEND_PARAMS {
    const char *str;
    const int strLen;

    const uint8_t size;
    const uint16_t color;
    const uint16_t bgColor;
    uint32_t yIndex;
    uint16_t *const buff;
    const int buffSize;
};

static uint16_t textDataChunk(void *paramBlock, const unsigned char **chunkAddress) {
    struct TEXT_SEND_PARAMS *params = paramBlock;
    if ((params->yIndex % params->size) == 0) {
        uint16_t pixelYIndex = params->yIndex / params->size;
        if (pixelYIndex >= CHAR_HEIGHT) {
            return 0;
        }
        uint8_t bitMask = 1u << pixelYIndex;
        for (int charIdx = 0, wordIdx = 0; charIdx < params->strLen; charIdx++) {
            unsigned char c = params->str[charIdx];
            c = ((c < 32) ? '?' : c) - 32;
            const unsigned char *symbolFontBytes = font[c];
            for (int dotIdx = 0; dotIdx < CHAR_WIDTH; dotIdx++) {
                uint16_t color = (symbolFontBytes[dotIdx] & bitMask) ? params->color : params->bgColor;
                for (int pxIdx = params->size - 1; pxIdx >= 0; pxIdx--) {
                    params->buff[wordIdx++] = color;
                }
            }
        }
    }
    *chunkAddress = (const unsigned char *) params->buff;
    params->yIndex++;
    return params->buffSize*2;
}

/*Draws a character (fonts imported from fonts.h) at X,Y location with specified font colour, size and Background colour*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void ILI9341_Draw_Char(char Character, uint8_t X, uint8_t Y, uint16_t Colour, uint16_t Size,
                       uint16_t Background_Colour) {
    ILI9341_Draw_Text_Len(&Character, 1, X, Y, Colour, Size, Background_Colour);
}

/*Draws an array of characters (fonts imported from fonts.h) at X,Y location with specified font colour, size and Background colour*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void ILI9341_Draw_Text_Len(const char *Text, uint8_t Len, uint8_t X, uint8_t Y, uint16_t Colour, uint16_t Size,
                           uint16_t Background_Colour) {
    int lenLimit = (LCD_WIDTH - X) / (CHAR_WIDTH * Size);
    if (lenLimit < Len) {
        Len = lenLimit;
    }
    size_t buffSize = CHAR_WIDTH * Len * Size;
    uint16_t buff[buffSize];
    uint16_t swapColour = (Colour >> 8u) | (Colour << 8u);
    uint16_t swapBgColour = (Background_Colour >> 8u) | (Background_Colour << 8u);
    struct TEXT_SEND_PARAMS textSendParams = {
            .bgColor= swapBgColour,
            .yIndex = 0,
            .color= swapColour,
            .size = Size,
            .buff = buff,
            .buffSize = buffSize,
            .str = Text,
            .strLen = Len
    };
    ILI9341_Draw_Box_By_Chunks(textDataChunk, &textSendParams, X, Y, CHAR_WIDTH * Size * Len, CHAR_HEIGHT * Size);
}

