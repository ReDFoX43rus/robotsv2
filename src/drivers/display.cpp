#include "display.h"

#include "string.h"
#include "math.h"

#define PI 3.14159265

COledDisplay::COledDisplay(uint16_t width, uint16_t height, Display_Controller type)
{
	m_Width = width;
	m_Height = height;
	m_Type = type;

	if (type == SSD1306)
	{
		m_Oled = new CSSD1306(width, height);
		m_Buffer = new uint8_t[width * height / 8];
		m_BufferSize = width * height / 8;
	}

	FillScreenBuff(0);
}
COledDisplay::~COledDisplay()
{
	if (m_Type == SSD1306)
	{
		delete m_Oled;
		delete m_Buffer;
	}
}

void COledDisplay::TestMode(uint8_t on)
{
	if(m_Type == SSD1306){
		if(on)
			m_Oled->AllPixOn();
		else m_Oled->AllPixRAM();
	}
}
void COledDisplay::SetBrightness(uint8_t value)
{
	if(m_Type == SSD1306){
		m_Oled->SetContrast(value);
	}
}
void COledDisplay::FillScreenBuff(uint8_t value)
{
	memset((void*)m_Buffer, value, m_BufferSize);
}
void COledDisplay::UpdateFromBuff()
{
	if(m_Type == SSD1306){
		m_Oled->DisplayFullUpdate(m_Buffer, m_BufferSize);
	}
}

void COledDisplay::DrawPixel(uint16_t x, uint16_t y, bool on){
	if(x >= m_Width || y >= m_Height)
		return;

	if(m_Type == SSD1306){
		uint16_t byteIDx = y >> 3;
		uint8_t bitIDx = y - (byteIDx << 3);

		byteIDx *= m_Width;
		byteIDx += x;

		if(on)
			m_Buffer[byteIDx] |= (1 << bitIDx);
		else m_Buffer[byteIDx] &= ~(1 << bitIDx);
	}
}

uint16_t COledDisplay::GetYFromStraight(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t currentX){
	if(x2 == x1)
		return y2;

	return (currentX - x1) / (x2 - x1) * (y2 - y1) + y2;
}

void COledDisplay::DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	const int16_t deltaX = abs(x2 - x1);
	const int16_t deltaY = abs(y2 - y1);
	const int16_t signX = x1 < x2 ? 1 : -1;
	const int16_t signY = y1 < y2 ? 1 : -1;

	int16_t error = deltaX - deltaY;

	DrawPixel(x2, y2, 1);

	while (x1 != x2 || y1 != y2)
	{
		DrawPixel(x1, y1, 1);
		const int16_t error2 = error * 2;

		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}
		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}
}
void COledDisplay::DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	DrawLine(x1, y1, x1, y2);
	DrawLine(x1, y2, x2, y2);
	DrawLine(x2, y2, x2, y1);
	DrawLine(x1, y1, x2, y1);
}

void COledDisplay::GetCirclePoints(uint16_t *destX, uint16_t *destY, uint16_t radius, uint16_t currentTime){
	*destX = radius * cos(currentTime * PI / 180.0);
	*destY = radius * sin(currentTime * PI / 180.0);
}

void COledDisplay::DrawCircle(uint16_t x, uint16_t y, uint16_t radius)
{
	uint16_t drawX = 0, drawY = 0;
	for(uint16_t i = 0; i < 360; i++){
		GetCirclePoints(&drawX, &drawY, radius, i);
		drawX += x;
		drawY += y;

		DrawPixel(drawX, drawY, true);
	}
}
uint8_t COledDisplay::DrawChar(uint8_t x, uint8_t y, uint8_t fontID, uint8_t chr)
{
	return 0;	
}
void COledDisplay::DrawString(uint8_t x, uint8_t y, uint8_t fontID, uint8_t *str)
{
}