#include "uart.h"
#include "i2c-ths.h"
#include "cconsole/console.h"
#include "filesystem/sdcard.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STARTUP_FILENAME "exec /fat/startup.sc"

extern "C" void initialize_filesystem(void);

extern "C" void app_main()
{
	initialize_filesystem();
	CSDCard sd(19, 22, 21, 18, 23, 16);
	// uart << "Mounting fat: " << sd.MountFat("/sdcard", true) << endl;

	/* Setup thread-safe i2c 
	 * We cannot set frequency higher since am2320 dont wanna work :c */
	i2c_master_setup(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, 100000);

	console.HandleCmd(uart, STARTUP_FILENAME);

	while(1)
		console.WaitForCmd(uart);

	/* Release thread-safe i2c and semaphore */
	i2c_release(I2C_NUM_0);
}
