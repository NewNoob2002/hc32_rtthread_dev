#include <rtthread.h>
#include <rthw.h>
#include "Arduino.h"

int main()
{
	pinMode(PA1, OUTPUT);
	while(1)
	{
		digitalToggle(PA1);
		rt_kprintf("Hello\n");
		rt_thread_mdelay(1000);
	}
}