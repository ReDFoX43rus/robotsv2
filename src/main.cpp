#include "uart.h"
#include "i2c-ths.h"
#include "cconsole/console.h"

#include "drivers/i2c-lcd1602.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "drivers/lcd_driver.h"

extern "C" void initialize_filesystem(void);

void test_lcd_driver()
{
	CLcdDriver lcd(0x27, true);
	lcd.Init();

	uart << "cursor on" << endl;
	lcd.Display(true, true, false);

	lcd.WriteString("Hello, Faramoz!");
	lcd.MoveCursor(1, 2);
	lcd.WriteString("Hello, Egor!");
}

extern "C" void app_main()
{
	initialize_filesystem();

	/* Setup thread-safe i2c */
	i2c_master_setup(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, 400000);

	console.HandleCmd(uart, "auth 123");
	console.HandleCmd(uart, "wifi init");
	console.HandleCmd(uart, "wifi connect Faramoz 12169931");
	console.HandleCmd(uart, "tcp 0");

	test_lcd_driver();

	while(1)
		console.WaitForCmd(uart);

	/* Release thread-safe i2c and semaphore */
	i2c_release(I2C_NUM_0);
}
