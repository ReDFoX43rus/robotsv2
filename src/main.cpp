#include "uart.h"
#include "i2c-ths.h"
#include "cconsole/console.h"

extern "C" void initialize_filesystem(void);

extern "C" void test_ruc_threadsv2(void);

extern "C" void hello_world(void *arg){
	vTaskDelay(pdMS_TO_TICKS(1000));
	vTaskDelete(NULL);
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

	// test_ruc_threadsv2();

	TaskHandle_t handle;
	xTaskCreate(hello_world, "NULL", 2048, NULL, 1, &handle);

	while(1)
		console.WaitForCmd(uart);

	/* Release thread-safe i2c and semaphore */
	i2c_release(I2C_NUM_0);
}
