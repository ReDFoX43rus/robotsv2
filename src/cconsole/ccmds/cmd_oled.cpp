#include "iobase/iobase.h"

#include "drivers/display.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void CmdOledHandler(CIOBase &io, int argc, char *argv[])
{
	COledDisplay display(128, 64);

	display.FillScreenBuff(0);

	/* Draw top line */
	display.DrawLine(0, 8, 127, 8);

	/* Draw rectangle */
	display.DrawRectangle(6, 10, 125, 62);

	/* Draw pixel */
	display.DrawPixel(127 / 2, 64 / 2, true);

	/* Draw circle */
	display.DrawCircle(127 / 2, 64 / 2, 15);

	/* Moar lines */
	display.DrawLine(0, 8, 127, 8);
	display.DrawLine(0, 8, 4, 62);

	display.DrawLine(10, 0, 10, 64);
	display.DrawLine(20, 0, 21, 64);	

	/* Commit */
	display.UpdateFromBuff();
}