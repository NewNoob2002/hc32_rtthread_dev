#include "Led.h"

#include "hal.h"

void hal::Led_HalInit()
{
    Led_Init();
}

void hal::Led_HalUpdate()
{
    Led_Update_Power();
    Led_Update_Charge();
    Led_Update_Function();
}
