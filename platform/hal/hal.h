#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>
#include <stdio.h>
#include "Arduino.h"
#include "mcu_config.h"
#ifdef __cplusplus

namespace hal
{
    void Init();
    void Update();

    /* Power */
    void Power_Init();
    void Power_OnCheck();
    void Power_SoftResetMonitor();
    /* Power key monitor */
		void PowerKey_Init();
	  void PowerKey_Monitor();
		void Power_SetKeyPressCountAdd();
		void Power_ResetKeyPressCount();
		void Power_LinuxSetPowerOff(bool flag);
    bool Power_IsLinuxPowerOff();
    /* Low battery power off */
    void Power_SetLowBatteryPowerOff(bool flag);
    bool Power_IsLowBatteryPowerOff();
    const char* Power_GetPowerOffCause();
    
    void Power_PowerOffMonitor();
    void Power_Update();
	
		/*Func Key*/
		void FuncKey_Init();
		void FuncKey_Monitor();
		key_action_t FuncKey_GetAction();

    /* I2C slave */
    void Slave_I2c_Init();
    void Slave_I2c_Deal();
//		void Slave_I2c_Check();
    /* Wire */
    void Wire_Init();
	
		/* LED */
		void Led_HalInit();
		void Led_HalUpdate();

    /* Display */
    void Display_Init();
    void Display_Update();
}

#endif

#endif // !__HAL_H__