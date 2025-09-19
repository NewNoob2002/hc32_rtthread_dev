#include "hal.h"

ledState powerLed = {0, 0};
ledState chargerLed = {0, 0};
ledState functionKeyLed = {0, 0};

void hal::Led_Init()
{
    pinMode(POWER_LED_PIN, OUTPUT);
    pinMode(CHARGE_LED_PIN, OUTPUT);
    pinMode(FUNCTION_LED_PIN, OUTPUT);

    digitalWrite(POWER_LED_PIN, LOW);
    digitalWrite(CHARGE_LED_PIN, LOW);
    digitalWrite(FUNCTION_LED_PIN, LOW);
}

void hal::PowerLED_Switch(int level)
{
	digitalWrite(POWER_LED_PIN, level);
}

void hal::ChargeLED_Switch(int level)
{
	digitalWrite(CHARGE_LED_PIN, level);
}

void hal::FuncLED_Switch(int level)
{
	digitalWrite(FUNCTION_LED_PIN, level);
}

void hal::Led_Update()
{
//		if(Power_Monitor.PowerOffReq)
//			return;
    if (powerLed.currentRate > 0)
    {
        const uint32_t now = millis();
        if (now - powerLed.lastToggleTime >= powerLed.currentRate)
        {
            powerLed.lastToggleTime = now;
            digitalToggle(POWER_LED_PIN);
        }
    }
		if (chargerLed.currentRate > 0)
    {
        const uint32_t now = millis();
        if (now - chargerLed.lastToggleTime >= chargerLed.currentRate)
        {
            chargerLed.lastToggleTime = now;
            digitalToggle(CHARGE_LED_PIN);
        }
    }
		if (functionKeyLed.currentRate > 0)
    {
        const uint32_t now = millis();
        if (now - functionKeyLed.lastToggleTime >= functionKeyLed.currentRate)
        {
            functionKeyLed.lastToggleTime = now;
            digitalToggle(FUNCTION_LED_PIN);
        }
    }
}
