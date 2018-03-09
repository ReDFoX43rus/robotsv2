#ifndef DRIVERS_LCDDRIVER_H
#define DRIVERS_LCDDRIVER_H

#include "i2c-ths.h"

class CLcdDriver {
public:
	CLcdDriver(uint8_t addr, bool backlight);
	~CLcdDriver();

	enum direction_t{
		LEFT,
		RIGHT
	};

	void Init();

	void Clear();
	void Home();
	void MoveCursor(uint8_t col, uint8_t row);
	void Backlight(bool blOn);
	void Display(bool displayOn, bool cursorOn, bool blinkingOn);
	void EntryMode(direction_t dir, bool autoScrool);
	void DisplayShift(direction_t dir);
	void MoveCursor(direction_t dir);

	void DefineChar(uint8_t index, uint8_t pixelmap[]);
	void WriteChar(uint8_t chr);
	void WriteString(const char *str);
private:
	uint8_t m_Addr;

	uint8_t m_BacklightFlag;
	uint8_t m_DisplayFlag;
	uint8_t m_EntryModeFlag;


	void Cmd(uint8_t cmd);
	void Data(uint8_t data);

	void WriteData(uint8_t data, uint8_t rs_flag);
	void WriteTop4Bits(uint8_t byte);
	void TrigEnable(uint8_t byte);
	void WriteToExpander(uint8_t byte);
	void SendByte(uint8_t byte);
};

#endif /* end of include guard: DRIVERS_LCDDRIVER_H */
