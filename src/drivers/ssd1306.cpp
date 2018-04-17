#include "ssd1306.h"
#include "i2c-ths.h"

/* Fundamental commands */
#define SSD1306_CMD_SetContrast 		0x81
#define SSD1306_CMD_AllPixRAM 			0xA4
#define SSD1306_CMD_AllPixOn 			0xA5
#define SSD1306_CMD_SetInverseOff 		0xA6
#define SSD1306_CMD_SetInverseOn 		0xA7
#define SSD1306_CMD_Sleep 				0xAE
#define SSD1306_CMD_Wake 				0xAF

/* Scrolling commands */
#define SSD1306_CMD_DeactivateScroll 	0x2E

/* Addressing commands */
/* Set auto scroll mode */
#define SSD1306_CMD_SetMemAdressingMode 0x20

/* Display range & scrolling range commands */
#define SSD1306_CMD_SetColumnAddr 		0x21
#define SSD1306_CMD_SetPageAddr 		0x22

/* Selecting page and range of auto shift ?? */
#define SSD1306_CMD_PageAddrMode_SetPage		 	0xB0
#define SSD1306_CMD_PageAddrMode_StartColumnLo 		0x00
#define SSD1306_CMD_PageAddrMode_StartColumnHi 		0x10

/* Hardware configuration */
#define SSD1306_CMD_SetDisplayStartLine 	0x40
#define SSD1306_CMD_SetSegmentRemap 		0xA0
#define SSD1306_CMD_SetMultiplexRatio 		0xA8
#define SSD1306_CMD_SetCOMoutScanDirection 	0xC0
#define SSD1306_CMD_SetDisplayOffset 		0xD3
#define SSD1306_CMD_SetCOMPinsConfig 		0xDA

/* Timing & Driving Scheme Setting  */
#define SSD1306_CMD_SetDisplayClockDivider 	0xD5
#define SSD1306_CMD_SetPrechargePeriod 		0xD9
#define SSD1306_CMD_SetVCOMHDeselectLevel 	0xDB

/* Charge Pump */
#define SSD1306_CMD_ChargePumpSetting 		0x8D

#define SSD1306_I2C_ADDR 					0x3C

CSSD1306::CSSD1306(uint8_t width, uint8_t height, AutoShiftMode shiftMode, IfType ifType)
{
	m_Width = width;
	m_Height = height;
	m_ShiftMode = shiftMode;
	m_IfType = ifType;

	Sleep();
	SetDisplayClockDivider(1, 8);
	SetMultiplexRatio(m_Height);
	SetDisplayOffset(0);
	SetDisplayStartLine(0);
	ChargePumpSetting(1);
	SetMemAdressingMode(ShiftHorVer);
	SetSegmentRemap(0);		   
	// SetCOMoutScanDirection(0);

	// if ((m_Width == 128) && (m_Height == 32))
	// 	SetCOMPinsConfig(0, 0);
	// else if ((m_Width == 128) && (m_Height == 64))
	// 	SetCOMPinsConfig(1, 0);
	// else if ((m_Width == 96) && (m_Height == 16))
	// 	SetCOMPinsConfig(0, 0);

	SetContrast(127);
	// SetPrechargePeriod(2, 2);
	// SetVCOMHDeselectLevel(0x40);
	AllPixRAM();
	SetInverse(false);
	DeactivateScroll();
	Wake();
}
CSSD1306::~CSSD1306()
{
}

void CSSD1306::SetResetPinConfig(bool resetPinUsed, gpio_num_t resetGPIO)
{
	m_ResetPinUsed = resetPinUsed;
	m_ResetGPIO = resetGPIO;

	assert("Using reset pin is not supported yet");
}

void CSSD1306::SendData(uint8_t *pBuff, uint16_t buffLen)
{
	uint8_t tmpBuff = 0x40; // Control byte (Co = 0, D/A = 1)

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, I2C_MASTER_NACK);
	i2c_master_write_byte(cmd, tmpBuff, I2C_MASTER_NACK);
	i2c_master_write(cmd, pBuff, buffLen, I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

void CSSD1306::SendCommand(uint8_t Command, uint8_t *pBuff, uint16_t buffLen)
{
	uint8_t tmpBuff[2];
	tmpBuff[0] = 0x00; // Control byte (Co = 0, D/A = 0)
	tmpBuff[1] = Command;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, I2C_MASTER_NACK);
	i2c_master_write(cmd, tmpBuff, 2, I2C_MASTER_NACK);

	if(pBuff && buffLen)
		i2c_master_write(cmd, pBuff, buffLen, I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

void CSSD1306::SetMemAdressingMode(AutoShiftMode mode)
{
	SendCommand(SSD1306_CMD_SetMemAdressingMode | mode , 0, 0);
}
void CSSD1306::SetPageForPageShift(uint8_t page)
{
	page &= 0x07;
	SendCommand(SSD1306_CMD_PageAddrMode_SetPage | page, 0, 0);
}
void CSSD1306::SetStartColumnForPageShift(uint8_t column)
{
	column &= 0x7F;
	SendCommand(SSD1306_CMD_PageAddrMode_StartColumnLo | (column & 0x07), 0, 0);
	SendCommand(SSD1306_CMD_PageAddrMode_StartColumnHi | (column >> 4), 0, 0);
}
void CSSD1306::SetDisplayStartLine(uint8_t line)
{
	line &= 0x3F;
	SendCommand(SSD1306_CMD_SetDisplayStartLine | line, 0, 0);
}
void CSSD1306::SetSegmentRemap(uint8_t value)
{
	SendCommand(SSD1306_CMD_SetSegmentRemap | (value > 0), 0, 0);
}
void CSSD1306::SetMultiplexRatio(uint8_t value)
{
	value--;
	value &= 0x3F;
	SendCommand(SSD1306_CMD_SetMultiplexRatio, &value, 1);
}
void CSSD1306::SetDisplayOffset(uint8_t offset)
{
	SendCommand(SSD1306_CMD_SetDisplayOffset, &offset, 1);
}
void CSSD1306::SetDisplayClockDivider(uint8_t clkdiv, uint8_t fosc)
{
	clkdiv--;
	clkdiv &= 0x0F;
	clkdiv |= ((fosc & 0x0F) << 4);
	SendCommand(SSD1306_CMD_SetDisplayClockDivider, &clkdiv, 1);
}

void CSSD1306::Sleep(void){
	SendCommand(SSD1306_CMD_Sleep, 0, 0);
}
void CSSD1306::Wake(void){
	SendCommand(SSD1306_CMD_Wake, 0, 0);
}
void CSSD1306::SetInverse(bool on){
	SendCommand(on ? SSD1306_CMD_SetInverseOn : SSD1306_CMD_SetInverseOff, 0, 0);
}
void CSSD1306::AllPixOn(void){
	SendCommand(SSD1306_CMD_AllPixOn, 0, 0);
}
void CSSD1306::AllPixRAM(void){
	SendCommand(SSD1306_CMD_AllPixRAM, 0, 0);
}
void CSSD1306::SetContrast(uint8_t Value){
	SendCommand(SSD1306_CMD_SetContrast, &Value, 1);
}
void CSSD1306::SetColumns(uint8_t Start, uint8_t End){
	Start &= 0x7F;
	End &= 0x7F;
	uint8_t Buff[] = {Start, End};
	SendCommand(SSD1306_CMD_SetColumnAddr, Buff, 2);
}
void CSSD1306::SetPages(uint8_t Start, uint8_t End){
	Start &= 0x07;
	End &= 0x07;
	uint8_t Buff[] = {Start, End};
	SendCommand(SSD1306_CMD_SetPageAddr, Buff, 2);
}
void CSSD1306::DisplayFullUpdate(uint8_t *pBuff, uint16_t BuffLen){
	SetColumns(0, m_Width - 1);
	SetPages(0, (m_Height >> 3) - 1);
	SendData(pBuff, BuffLen);
}
void CSSD1306::DeactivateScroll(){
	SendCommand(SSD1306_CMD_DeactivateScroll, 0, 0);
}
void CSSD1306::ChargePumpSetting(uint8_t Value)
{
	Value = Value ? 0x14 : 0x10;
	SendCommand(SSD1306_CMD_ChargePumpSetting, &Value, 1);
}