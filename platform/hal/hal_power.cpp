#include "power_control.h"
#include "charger_control_new.h"
#include "charger_control.h"
#include "battery.h"
#include "Led.h"
#include "ui/ui.h"

#include "hal.h"
#include <hc32f460_rmu.h>

#define FORCE_SHUTDOWN() (Power_Monitor.Force_ShutDown_Funckey && Power_Monitor.Force_ShutDown_Powerkey)

Power_Monitor_t Power_Monitor;
BatteryState batteryState;

static void Charging_State_Indication(void)
{
  const uint32_t now = millis();
  if (now - Power_Monitor.BatteryLastHandleTime > 2000)
  {
    Power_Monitor.BatteryLastHandleTime = now;
    WatchDog_feed();
    if (online_devices.bq40z50)
    {
      checkBatteryLevels(&batteryState);
    }
  }
  if (now - Power_Monitor.ChargerLastHandleTime > 300)
  {
    Power_Monitor.ChargerLastHandleTime = now;
    if (!online_devices.mp2762)
    {
      Charger_Control_Monitor(&batteryState);
    }
    else
    {
      mp2762a_update(&batteryState);
    }
  }
}

void hal::Power_Init()
{
  memset(&Power_Monitor, 0, sizeof(Power_Monitor));

  Power_Control_Init();
	Charger_Control_GPIO_Init();
	USB_Switch_GPIO_Init();
  WatchDog_Init();
  PowerKey_Init();

  RMU_GetResetCause(&Power_Monitor.cause);
  if (Power_Monitor.cause.enSoftware == Set)
  {
    CORE_DEBUG_PRINTF("Reset cause: software\n");
    POWER_ENABLE();
  }
  else
  {
    POWER_DISABLE();
  }
}

void hal::Power_OnCheck()
{
  Power_Monitor.mcu_powerOn_Done = false;
  loadScreen(SCREEN_ID_POWER_OFF_CHARGE);
  while (1)
  {
    if (Power_Monitor.cause.enSoftware == Set)
    {
      break;
    }
    hal::PowerKey_Monitor();
    Charging_State_Indication();
    Led_Update();
    hal::Display_Update();
  }
  /*System Power Trun ON*/
  POWER_ENABLE();
  CORE_DEBUG_PRINTF("POWER ON .................\n");
  Power_Monitor.mcu_powerOn_Done = true;
  Power_ResetKeyPressCount();
  Led_Power_switch(1);
  Led_Function_switch(1);
}

void hal::Power_Update()
{
  const uint32_t now = millis();
  if (now - Power_Monitor.BatteryLastHandleTime > 3000)
  {
    Power_Monitor.BatteryLastHandleTime = now;
    WatchDog_feed();
    if (!online_devices.bq40z50)
    {
      while (1)
      {
        WatchDog_feed();
        powerLed.currentRate = 100;
        chargerLed.currentRate = 100;
        Led_Update_Power();
        Led_Update_Charge();
        Led_Update_Function();
      }
    }
    else
    {
      checkBatteryLevels(&batteryState);
    }
  }
  if (now - Power_Monitor.ChargerLastHandleTime > 600)
  {
    Power_Monitor.ChargerLastHandleTime = now;
    if (!online_devices.mp2762)
    {
      Charger_Control_Monitor(&batteryState);
    }
    else
    {
      mp2762a_update(&batteryState);
    }
    // #ifdef __CORE_DEBUG
    //         char buffer[128];
    //         snprintf(buffer, sizeof(buffer),
    //                  "Batt(%d%%): %0.02fV Type:%s Temp:%0.02f C\n",
    //                  batteryState.Percent, batteryState.Voltage/ 1.0,
    //                  getBatteryChargeStatus(batteryState.chargeStatus), (batteryState.Temp/ 10.0 - 273.15));
    //         CORE_DEBUG_PRINTF("%s", buffer);
    // #endif
  }
}

void hal::Power_SoftResetMonitor()
{
  if (systemInfo.sw_reset_flag)
  {
    delay_ms(1200);
    digitalWrite(PC13, LOW);
    NVIC_SystemReset();
  }
}

void hal::Power_PowerOffMonitor()
{
  if ((Power_Monitor.PowerOFF_Ensure && systemInfo.sync_flag) || FORCE_SHUTDOWN())
  {
    if (FORCE_SHUTDOWN())
    {
      CORE_DEBUG_PRINTF("Force Shutdown\n");
      lv_label_set_text(objects.label_power_off, "Force Shutdown");
      loadScreen(SCREEN_ID_POWER_OFF);
      hal::Display_Update();
    }
    USB_Switch_GPIO_Control(1);
    delay_ms(9000);

    if (Power_IsLinuxPowerOff())
    {
      delay_ms(2000);
    }
    Led_Power_switch(0);
    Led_Function_switch(0);
    CORE_DEBUG_PRINTF("POWER OFF .................\n");
    CORE_DEBUG_PRINTF("Power Off Cause: %s\n", Power_GetPowerOffCause());
    Power_ResetKeyPressCount();
    Power_LinuxSetPowerOff(false);
    Power_SetLowBatteryPowerOff(false);
    Power_Monitor.PowerOffReq = false;
    POWER_DISABLE();
    delay_ms(100);
		delay_init();
    loadScreen(SCREEN_ID_POWER_OFF_CHARGE);
    while (1)
    {
      hal::PowerKey_Monitor();
      Charging_State_Indication();
      Led_Update();
      hal::Display_Update();
    }
  }
  //	if(Power_Monitor.PowerKey_PressCount ==50)
  //	{
  //		loadScreen(SCREEN_ID_POWER_OFF_CHARGE);
  //		while(1)
  //		{
  //			hal::PowerKey_Monitor();
  //			hal::Display_Update();
  //			if(Power_Monitor.PowerKey_PressCount == 0)
  //				break;
  //		}
  //	}
  if (Power_Monitor.PowerKey_PressCount >= 200 || Power_IsLinuxPowerOff() || Power_IsLowBatteryPowerOff())
  {
    Power_Monitor.mcu_powerOn_Done = false;
    Led_Charge_switch(0);
    Led_Power_switch(1);
    Led_Function_switch(1);
    systemInfo.power_off_flag = 1;
    Power_Monitor.PowerOffReq = true;
  }
}

void hal::Power_SetKeyPressCountAdd()
{
  Power_Monitor.PowerKey_PressCount++;
}

void hal::Power_ResetKeyPressCount()
{
  Power_Monitor.PowerKey_PressCount = 0;
}

void hal::Power_LinuxSetPowerOff(bool flag)
{
  Power_Monitor.LinuxPowerOff = flag;
}

bool hal::Power_IsLinuxPowerOff()
{
  return Power_Monitor.LinuxPowerOff;
}

void hal::Power_SetLowBatteryPowerOff(bool flag)
{
  Power_Monitor.LowBatteryPowerOff = flag;
}

bool hal::Power_IsLowBatteryPowerOff()
{
  return Power_Monitor.LowBatteryPowerOff;
}

const char *hal::Power_GetPowerOffCause()
{
  if (Power_IsLinuxPowerOff())
  {
    return "LinuxPowerOff";
  }
  else if (Power_IsLowBatteryPowerOff())
  {
    return "LowBatteryPowerOff";
  }
  else if (FORCE_SHUTDOWN())
    return "Force_ShutDown";
  return "PowerKey";
}