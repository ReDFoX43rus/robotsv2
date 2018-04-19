#pragma once

/* Special thanks to Nadyrshin Ruslan
 * Youtube: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ 
 * Bitbucket: https://bitbucket.org/nadyrshin_ryu */

#include "driver/gpio.h"

/* Implemented in I2C mode
 * TODO: Implement spi & parallel modes */
class CSSD1306
{
public:
	enum IfType
	{
		IFTYPE_PAR8080,
		IFTYPE_PAR6880,
		IFTYPE_SPI_3W,
		IFTYPE_SPI_4W,
		IFTYPE_I2C
	};

	enum AutoShiftMode
	{
		ShiftHorVer, // First increment pointer horizontally, then Vertically
		ShiftVerHor, // First increment pointer vertically, then Horizontally
		ShiftPage	// Only horizontally increment
	};

	CSSD1306(uint8_t width, uint8_t height, AutoShiftMode shiftMode = ShiftHorVer, IfType ifType = IFTYPE_I2C);
	~CSSD1306();

	void SetResetPinConfig(bool resetPinUsed, gpio_num_t resetGPIO);

	void Sleep(void);
	void Wake(void);
	void SetInverse(bool on);

	/* Turn on all pixels on display (test mode) */
	void AllPixOn(void);

	/* Turn off test mode, display pixels from RAM buffer */
	void AllPixRAM(void);

	/* Set contrast value */
	void SetContrast(uint8_t Value);

	/* Set beginning and ending index of column to autoshift */
	void SetColumns(uint8_t Start, uint8_t End);

	/* Set beginning and ending index of page to autoshift */
	void SetPages(uint8_t Start, uint8_t End);

	/* Set display's RAM buffer as pBuff */
	void DisplayFullUpdate(uint8_t *pBuff, uint16_t BuffLen);

	/* Deactivate scroll */
	void DeactivateScroll();

private:
	uint8_t m_Width;
	uint8_t m_Height;

	IfType m_IfType;
	AutoShiftMode m_ShiftMode;

	/* Wether reset pin is used */
	bool m_ResetPinUsed;
	/* Reset pin configuration */
	gpio_num_t m_ResetGPIO;

	/* Send array of bytes to SSD1306 controller */
	void SendData(uint8_t *pBuff, uint16_t buffLen);

	/* Set autoshift mode in SSD1306's frame buffer */
	void SetMemAdressingMode(AutoShiftMode mode);

	/* Set page for autoshift mode: page */
	void SetPageForPageShift(uint8_t page);

	/* Set start column for autoshift mode: page */
	void SetStartColumnForPageShift(uint8_t column);

	/* Set start line from 0 to 63 */
	void SetDisplayStartLine(uint8_t line);

	/* value - 0 => column 0 is mapped to SEG0
	 * value - 1 => column 127 is mapped to SEG0 */
	void SetSegmentRemap(uint8_t value);

	void SetMultiplexRatio(uint8_t value);

	/* Vertical display cursor offset */
	void SetDisplayOffset(uint8_t offset);

	void SetDisplayClockDivider(uint8_t clkdiv, uint8_t fosc);

	/* Send command */
	void SendCommand(uint8_t cmd, uint8_t *pBuff, uint16_t buffLen);

	void ChargePumpSetting(uint8_t Value);

	void SetCOMoutScanDirection(uint8_t value);

	void SetCOMPinsConfig(uint8_t altComPinConfig, uint8_t leftRightRemap);

	void SetPrechargePeriod(uint8_t phase1Period, uint8_t phase2Period);

	void SetVCOMHDeselectLevel(uint8_t code);
};