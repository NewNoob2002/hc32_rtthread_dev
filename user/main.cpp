#include "hal.h"

en_result_t i2cSlave_Initialize();
en_result_t I2C_Slave_Receive_IT();
size_t i2cSlave_rxbuf_available();
int i2cSlave_rxbuf_read();
int i2cSlave_rxbuf_readBytes(uint8_t *data, size_t size);

static rt_thread_t led_thread = nullptr;
static rt_uint8_t led_thread_priority = 6;

static void led_thread_entry(void *para)
{
	INFO_PRINTF("Entry %s", __func__);
	while (1)
	{
		digitalToggle(PA1);
		INFO_PRINTF("available:%d", i2cSlave_rxbuf_available());
		rt_thread_mdelay(1000);
	}
}

int main()
{
	hal::Init();
	pinMode(PA1, OUTPUT);
	pinMode(MCU_ON_OFF_BATTERY_PIN, OUTPUT);
	digitalWrite(MCU_ON_OFF_BATTERY_PIN, HIGH);
	if(i2cSlave_Initialize() == Ok)
	{
		I2C_Slave_Receive_IT();
	}
	led_thread = rt_thread_create("led_thread",
								  led_thread_entry,
								  RT_NULL,
								  1024,
								  led_thread_priority,
								  100);
	rt_thread_startup(led_thread);
	while (1)
	{
		rt_thread_mdelay(1);
	}
}