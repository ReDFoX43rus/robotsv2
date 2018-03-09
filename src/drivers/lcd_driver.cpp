#include "lcd_driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define TIMEOUT_MS pdMS_TO_TICKS(500)

// Delays (microseconds)
#define DELAY_POWER_ON            50000  // wait at least 40us after VCC rises to 2.7V
#define DELAY_INIT_1               4500  // wait at least 4.1ms (fig 24, page 46)
#define DELAY_INIT_2               4500  // wait at least 4.1ms (fig 24, page 46)
#define DELAY_INIT_3                120  // wait at least 100us (fig 24, page 46)

#define DELAY_CLEAR_DISPLAY        2000
#define DELAY_RETURN_HOME          2000

#define DELAY_ENABLE_PULSE_WIDTH      1  // enable pulse must be at least 450ns wide
#define DELAY_ENABLE_PULSE_SETTLE    50  // command requires > 37us to settle (table 6 in datasheet)


// Commands
#define COMMAND_CLEAR_DISPLAY       0x01
#define COMMAND_RETURN_HOME         0x02
#define COMMAND_ENTRY_MODE_SET      0x04
#define COMMAND_DISPLAY_CONTROL     0x08
#define COMMAND_SHIFT               0x10
#define COMMAND_FUNCTION_SET        0x20
#define COMMAND_SET_CGRAM_ADDR      0x40
#define COMMAND_SET_DDRAM_ADDR      0x80

// COMMAND_ENTRY_MODE_SET flags
#define FLAG_ENTRY_MODE_SET_ENTRY_INCREMENT       0x02
#define FLAG_ENTRY_MODE_SET_ENTRY_DECREMENT       0x00
#define FLAG_ENTRY_MODE_SET_ENTRY_SHIFT_ON        0x01
#define FLAG_ENTRY_MODE_SET_ENTRY_SHIFT_OFF       0x00

// COMMAND_DISPLAY_CONTROL flags
#define FLAG_DISPLAY_CONTROL_DISPLAY_ON  0x04
#define FLAG_DISPLAY_CONTROL_DISPLAY_OFF 0x00
#define FLAG_DISPLAY_CONTROL_CURSOR_ON   0x02
#define FLAG_DISPLAY_CONTROL_CURSOR_OFF  0x00
#define FLAG_DISPLAY_CONTROL_BLINK_ON    0x01
#define FLAG_DISPLAY_CONTROL_BLINK_OFF   0x00

// COMMAND_SHIFT flags
#define FLAG_SHIFT_MOVE_DISPLAY          0x08
#define FLAG_SHIFT_MOVE_CURSOR           0x00
#define FLAG_SHIFT_MOVE_LEFT             0x04
#define FLAG_SHIFT_MOVE_RIGHT            0x00

// COMMAND_FUNCTION_SET flags
#define FLAG_FUNCTION_SET_MODE_8BIT      0x10
#define FLAG_FUNCTION_SET_MODE_4BIT      0x00
#define FLAG_FUNCTION_SET_LINES_2        0x08
#define FLAG_FUNCTION_SET_LINES_1        0x00
#define FLAG_FUNCTION_SET_DOTS_5X10      0x04
#define FLAG_FUNCTION_SET_DOTS_5X8       0x00

// Control flags
#define FLAG_BACKLIGHT_ON    0b00001000      // backlight enabled (disabled if clear)
#define FLAG_BACKLIGHT_OFF   0b00000000      // backlight disabled
#define FLAG_ENABLE          0b00000100
#define FLAG_READ            0b00000010      // read (write if clear)
#define FLAG_WRITE           0b00000000      // write
#define FLAG_RS_DATA         0b00000001      // data (command if clear)
#define FLAG_RS_COMMAND      0b00000000      // command

#define LCD_DRIVER_NUM_ROWS               2            ///< Maximum number of supported rows for this device
#define LCD_DRIVER_NUM_COLUMNS            40           ///< Maximum number of supported columns for this device
#define LCD_DRIVER_NUM_VISIBLE_COLUMNS    16           ///< Number of columns visible at any one time

CLcdDriver::CLcdDriver(uint8_t addr, bool backlight){
	m_Addr = addr;
	m_BacklightFlag = backlight ? FLAG_BACKLIGHT_ON : FLAG_BACKLIGHT_OFF;
	m_DisplayFlag = FLAG_DISPLAY_CONTROL_DISPLAY_ON | FLAG_DISPLAY_CONTROL_CURSOR_OFF | FLAG_DISPLAY_CONTROL_BLINK_OFF;
	m_EntryModeFlag = FLAG_ENTRY_MODE_SET_ENTRY_INCREMENT | FLAG_ENTRY_MODE_SET_ENTRY_SHIFT_OFF;
}
CLcdDriver::~CLcdDriver(){

}

void CLcdDriver::Init(){
	ets_delay_us(DELAY_POWER_ON);

	WriteToExpander(0);
	ets_delay_us(1000);

	WriteTop4Bits(0x03 << 4);
	ets_delay_us(DELAY_INIT_1);
	WriteTop4Bits(0x03 << 4);
	ets_delay_us(DELAY_INIT_2);
	WriteTop4Bits(0x03 << 4);
	ets_delay_us(DELAY_INIT_3);
	WriteTop4Bits(0x02 << 4);

	// now we can use the command()/write() functions
	Cmd(COMMAND_FUNCTION_SET | FLAG_FUNCTION_SET_MODE_4BIT | FLAG_FUNCTION_SET_LINES_2 | FLAG_FUNCTION_SET_DOTS_5X8);

	Cmd(COMMAND_DISPLAY_CONTROL | m_DisplayFlag);

	Clear();

	Cmd(COMMAND_ENTRY_MODE_SET | m_EntryModeFlag);

	Home();
}

void CLcdDriver::Clear(){
	Cmd(COMMAND_CLEAR_DISPLAY);
	ets_delay_us(DELAY_CLEAR_DISPLAY);
}
void CLcdDriver::Home(){
	Cmd(COMMAND_RETURN_HOME);
	ets_delay_us(DELAY_RETURN_HOME);
}
void CLcdDriver::MoveCursor(uint8_t col, uint8_t row){
	if(col > LCD_DRIVER_NUM_COLUMNS || row > LCD_DRIVER_NUM_ROWS || !col || !row)
		return;

	Cmd(COMMAND_SET_DDRAM_ADDR | (col + (row == 1 ? 0x0 : 0x40) - 1));
}
void CLcdDriver::Backlight(bool blOn){
	m_BacklightFlag = blOn ? FLAG_BACKLIGHT_ON : FLAG_BACKLIGHT_OFF;
	WriteToExpander(0);
}
void CLcdDriver::Display(bool displayOn, bool cursorOn, bool blinkingOn){
	if(displayOn)
		m_DisplayFlag |= FLAG_DISPLAY_CONTROL_DISPLAY_ON;
	else m_DisplayFlag &= ~FLAG_DISPLAY_CONTROL_DISPLAY_ON;

	if(cursorOn)
		m_DisplayFlag |= FLAG_DISPLAY_CONTROL_CURSOR_ON;
	else m_DisplayFlag &= ~FLAG_DISPLAY_CONTROL_CURSOR_ON;

	if(blinkingOn)
		m_DisplayFlag |= FLAG_DISPLAY_CONTROL_BLINK_ON;
	else m_DisplayFlag &= ~FLAG_DISPLAY_CONTROL_BLINK_ON;

	Cmd(COMMAND_DISPLAY_CONTROL | m_DisplayFlag);
}
void CLcdDriver::EntryMode(direction_t dir, bool autoScrool){
	if(dir == CLcdDriver::LEFT)
		m_EntryModeFlag &= ~FLAG_ENTRY_MODE_SET_ENTRY_INCREMENT;
	else m_EntryModeFlag |= FLAG_ENTRY_MODE_SET_ENTRY_INCREMENT;

	if(autoScrool)
		m_EntryModeFlag |= FLAG_ENTRY_MODE_SET_ENTRY_SHIFT_ON;
	else m_EntryModeFlag &= ~FLAG_ENTRY_MODE_SET_ENTRY_SHIFT_ON;

	Cmd(COMMAND_ENTRY_MODE_SET | m_EntryModeFlag);
}
void CLcdDriver::DisplayShift(direction_t dir){
	uint8_t flag = dir == CLcdDriver::LEFT ? FLAG_SHIFT_MOVE_LEFT : FLAG_SHIFT_MOVE_RIGHT;
	Cmd(COMMAND_SHIFT | FLAG_SHIFT_MOVE_DISPLAY | flag);
}
void CLcdDriver::MoveCursor(direction_t dir){
	uint8_t flag = dir == CLcdDriver::LEFT ? FLAG_SHIFT_MOVE_RIGHT : FLAG_SHIFT_MOVE_LEFT;
	Cmd(COMMAND_SHIFT | FLAG_SHIFT_MOVE_CURSOR | flag);
}

void CLcdDriver::DefineChar(uint8_t index, uint8_t pixelmap[]){
	index &= 0x07;

	Cmd(COMMAND_SET_CGRAM_ADDR | (index << 3));

	for(int i = 0; i < 8; i++){
		Data(pixelmap[i]);
	}
}
void CLcdDriver::WriteChar(uint8_t chr){
	Data(chr);
}
void CLcdDriver::WriteString(const char *str){
	for(int i = 0; str[i]; i++)
		Data(str[i]);
}

inline void CLcdDriver::Cmd(uint8_t cmd){
	WriteData(cmd, FLAG_RS_COMMAND);
}
inline void CLcdDriver::Data(uint8_t data){
	WriteData(data, FLAG_RS_DATA);
}

inline void CLcdDriver::WriteData(uint8_t data, uint8_t rs_flag){
	WriteTop4Bits((data & 0xF0) | rs_flag);
	WriteTop4Bits(((data & 0x0F) << 4) | rs_flag);
}

inline void CLcdDriver::WriteTop4Bits(uint8_t byte){
	WriteToExpander(byte);
	TrigEnable(byte);
}

void CLcdDriver::TrigEnable(uint8_t byte){
	WriteToExpander(byte | FLAG_ENABLE);
	ets_delay_us(DELAY_ENABLE_PULSE_WIDTH);
	WriteToExpander(byte & ~FLAG_ENABLE);
	ets_delay_us(DELAY_ENABLE_PULSE_SETTLE);
}

inline void CLcdDriver::WriteToExpander(uint8_t byte){
	SendByte(byte | m_BacklightFlag);
}

void CLcdDriver::SendByte(uint8_t byte){
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, m_Addr << 1 | I2C_MASTER_WRITE, I2C_MASTER_ACK);
	i2c_master_write_byte(cmd, byte, I2C_MASTER_ACK);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, TIMEOUT_MS);
	i2c_cmd_link_delete(cmd);
}
