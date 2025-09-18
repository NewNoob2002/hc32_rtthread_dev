#include <ButtonEvent/ButtonEvent.h>

#include "hal.h"

static ButtonEvent FuncKey;
static volatile uint8_t ForceShutdown_funckey = 0;
static volatile key_action_t g_funckey_action = KEY_ACTION_NONE;

static void FuncKey_callback(ButtonEvent *btn, int event)
{
    switch (event)
    {
    case ButtonEvent::EVENT_DOUBLE_CLICKED:
        g_funckey_action = KEY_ACTION_DOUBLE_CLICK;
        break;
    case ButtonEvent::EVENT_CLICKED:
        g_funckey_action = KEY_ACTION_SINGLE_CLICK;
        break;
		case ButtonEvent::EVENT_RELEASED:
				ForceShutdown_funckey = 0;
				Power_Monitor.Force_ShutDown_Funckey = false;
				break;
		case ButtonEvent::EVENT_LONG_PRESSED_REPEAT:
				//CORE_DEBUG_PRINTF("FUNC_LONG_R\n");
				ForceShutdown_funckey++;
				if(ForceShutdown_funckey >= 4)
					Power_Monitor.Force_ShutDown_Funckey = true;
        break;
//		default:
//				CORE_DEBUG_PRINTF("FUNC_DEF\n");
//				ForceShutdown_funckey = 0;
//				Power_Monitor.Force_ShutDown_Funckey = false;
//				break;
    }
		//CORE_DEBUG_PRINTF("FUNC_KEYL:%d, Force:%d\n", Power_Monitor.PowerKey_PressCount, ForceShutdown_funckey);
}

void hal::FuncKey_Init()
{
    pinMode(FUNCTION_KEY_PIN, INPUT_PULLUP);
    FuncKey.EventAttach(FuncKey_callback);
}

void hal::FuncKey_Monitor()
{
    FuncKey.EventMonitor(digitalRead(FUNCTION_KEY_PIN) == LOW);
}

key_action_t hal::FuncKey_GetAction()
{
	  key_action_t action = g_funckey_action;
    g_funckey_action = KEY_ACTION_NONE;
    return action;
}