#include "iobase/iobase.h"
#include "drivers/lcd_driver.h"
#include "string.h"
#include "time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static CLcdDriver lcd(0x27, true);

static void PrintTimeTask(void *arg){
	while(1){
		uint32_t seconds = clock() / CLOCKS_PER_SEC;
		uint32_t minutes = seconds / 60;
		uint32_t hours = minutes / 60;
		uint32_t days = hours / 24;

		seconds %= 60;
		minutes %= 60;
		hours %= 24;

		lcd.Home();

		char buff[64];
		sprintf(buff, "Uptime: %d days", days);

		lcd.WriteString(buff);

		lcd.MoveCursor(1, 2);
		sprintf(buff, "%02d:%02d:%02d", hours, minutes, seconds);
		lcd.WriteString(buff);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void CmdLcdHandler(CIOBase &io, int argc, char *argv[]){
	if(argc < 2){
		io << "Usage: " << argv[0] << " cmd" << endl;
		return;
	}

	char *cmd = argv[1];

	if(!strcmp(cmd, "write")){
		for(int i = 2; i < argc; i++){
			lcd.WriteString(argv[i]);
			if(i != argc - 1)
				lcd.WriteChar(' ');
		}
	} else if(!strcmp(cmd, "home"))
		lcd.Home();
	else if (!strcmp(cmd, "clear"))
		lcd.Clear();
	else if (!strcmp(cmd, "scroll"))
		lcd.EntryMode(CLcdDriver::RIGHT, !strcmp(argv[2], "on"));
	else if (!strcmp(cmd, "writechar"))
		lcd.WriteChar(argv[2][0]);
	else if (!strcmp(cmd, "init"))
		lcd.Init();
	else if (!strcmp(cmd, "pos"))
		lcd.MoveCursor(atoi(argv[2]), atoi(argv[3]));
	else if (!strcmp(cmd, "uptime")){
		TaskHandle_t handle;
		xTaskCreate(PrintTimeTask, "uptime_task", 4096, NULL, 4, &handle);
	}
}
