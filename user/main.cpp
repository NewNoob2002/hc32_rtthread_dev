#include <rtthread.h>
#include <rthw.h>
#include "Arduino.h"

int main()
{
	pinMode(PA1, OUTPUT);
//	uint8_t *p_test = (uint8_t *)rt_malloc(128);
//	if(p_test)
//	{
//		INFO_PRINTF("malloc done");
//	}
//	else
//	{
//		ERROR_PRINTF("malloc failed");
//	}
	
	while(1)
	{
		digitalToggle(PA1);
		rt_kprintf("Hello\n");
		rt_thread_mdelay(1000);
	}
}