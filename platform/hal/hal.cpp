//#include <MillisTaskManager/MillisTaskManager.h>
#include "hal.h"
/*GLOBAL*/
online_device online_devices;
SystemInfo_t systemInfo;

volatile uint32_t IIC_ERROR_COUNT = 0;
volatile uint32_t LOW_POWER_COUNT = 0;
extern "C" void systick_handle()
{
    IIC_ERROR_COUNT++;
}

void HAL_50msTimerInterrputUpdate()
{
    hal::Power_Update();
		hal::Power_SoftResetMonitor();
    hal::Power_PowerOffMonitor();
    hal::Led_HalUpdate();
}

void hal::Init()
{
		MEM_ZERO_STRUCT(systemInfo);
		MEM_ZERO_STRUCT(online_devices);
		MEM_ZERO_STRUCT(batteryState);
    Core_Init();
    Led_HalInit();
    Power_Init();
    Wire_Init();
    FuncKey_Init();
    /*Setup Core Init Check if Type-c power*/
    Power_OnCheck();
    Slave_I2c_Init();
		Display_Init();
    delay_ms(1500);
}
