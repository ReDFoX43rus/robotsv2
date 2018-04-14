/* LCD driver for i2c-connected 1602 displays */

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

	/* Dedicated setup function
	 * since there are some delays that cant be placed in ctro */
	void Init();

	/* Clear display */
	void Clear();

	/* Set cursor position at (1, 1) */
	void Home();

	/* Move cursor to specified position
	 * Range cols (including boundaries): from 1 to 16
	 * Range row (including boundaries): from 1 to 2 */
	void MoveCursor(uint8_t col, uint8_t row);

	/* Set backlight option */
	void Backlight(bool blOn);

	/* Set display options */
	void Display(bool displayOn, bool cursorOn, bool blinkingOn);

	/* dir: RIGHT for left-to-right languages
	 		LEFT for right-to-left languages */
	void EntryMode(direction_t dir, bool autoScrool);

	/* Shift display  */
	void DisplayShift(direction_t dir);

	/* Shift cursor */
	void MoveCursor(direction_t dir);

	/* Define custom char */
	void DefineChar(uint8_t index, uint8_t pixelmap[]);

	/* Print char on display */
	void WriteChar(uint8_t chr);

	/* Print 0-terminated string on display */
	void WriteString(const char *str);
private:
	uint8_t m_Addr;

	uint8_t m_BacklightFlag;
	uint8_t m_DisplayFlag;
	uint8_t m_EntryModeFlag;

	/* Send command to display */
	void Cmd(uint8_t cmd);

	/* Send data to display */
	void Data(uint8_t data);

	/* Helping functions, check out display datasheet */
	void WriteData(uint8_t data, uint8_t rs_flag);
	void WriteTop4Bits(uint8_t byte);
	void TrigEnable(uint8_t byte);
	void WriteToExpander(uint8_t byte);
	void SendByte(uint8_t byte);
};

#endif /* end of include guard: DRIVERS_LCDDRIVER_H */
