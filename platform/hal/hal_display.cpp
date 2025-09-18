#include "LCD.h"
#include "MYSPI.h"
#include "ui/ui.h"

#include "hal.h"

void hal::Display_Init()
{
    CORE_DEBUG_PRINTF("Display: init...");
    show_logo();
    CORE_DEBUG_PRINTF("done");
}

void hal::Display_Update()
{
		hal::Slave_I2c_Deal();
		ui_tick();
		lv_task_handler();
}
