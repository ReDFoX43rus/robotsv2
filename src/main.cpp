#include "uart.h"
#include "i2c-ths.h"
#include "cconsole/console.h"
#include "sensors/handler_c.h"

#define STARTUP_FILENAME "exec /fat/startup.sc"

extern "C" void initialize_filesystem(void);

extern "C" void app_main()
{
	initialize_filesystem();

	/* Setup thread-safe i2c 
	 * We cannot set frequency higher since am2320 dont wanna work :c */
	i2c_master_setup(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, 100000);

	/* Offline servo */
	set_voltage(22, 0);
	set_voltage(21, 0);
	set_voltage(17, 0); //(12, 0);
	set_voltage(13, 0);

	console.HandleCmd(uart, STARTUP_FILENAME);

	while(1)
		console.WaitForCmd(uart);

	/* Release thread-safe i2c and semaphore */
	i2c_release(I2C_NUM_0);
}
