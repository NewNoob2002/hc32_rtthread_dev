#include <rtthread.h>
#include <rthw.h>
#include <Arduino.h>


int boardInit()
{
	Serial.begin(115200);
	return 1;
}

INIT_BOARD_EXPORT(boardInit);