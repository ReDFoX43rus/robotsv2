#pragma once

#include "inttypes.h"
#include "ssd1306.h"

class COledDisplay
{
public:
	enum Display_Controller
	{
		SSD1306
	};

	COledDisplay(uint16_t width, uint16_t height, Display_Controller type = SSD1306);
	~COledDisplay();

	void TestMode(uint8_t on);
	void SetBrightness(uint8_t value);
	void FillScreenBuff(uint8_t value);
	void UpdateFromBuff();

	void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void DrawCircle(uint16_t x1, uint16_t x2, uint16_t radius);

	uint8_t DrawChar(uint8_t x, uint8_t y, uint8_t fontID, uint8_t chr);
	void DrawString(uint8_t x, uint8_t y, uint8_t fontID, uint8_t *str);
	void DrawPixel(uint16_t x, uint16_t y, bool on);
private:
	uint16_t m_Width;
	uint16_t m_Height;
	Display_Controller m_Type;

	uint8_t *m_Buffer;
	uint16_t m_BufferSize;

	CSSD1306 *m_Oled;

	static uint16_t GetYFromStraight(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t currentX);
	static void GetCirclePoints(uint16_t *destX, uint16_t *destY, uint16_t radius, uint16_t currentTime);
};