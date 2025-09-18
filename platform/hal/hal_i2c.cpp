#include "i2c_slave.h"
#include "battery.h"
#include "charger_control_new.h"

#include "hal.h"

void hal::Slave_I2c_Init()
{
    slave_i2c_get_irqn();
    if (Slave_Initialize() != Ok)
    {
        CORE_DEBUG_PRINTF("I2c_slave Init failed\n");
    }
}

void hal::Slave_I2c_Deal()
{
    IIC_deal();
	  if (IIC_ERROR_COUNT >= 5000)
    {
				//CORE_DEBUG_PRINTF("ERR_I2C\n");
        Slave_Initialize();
        Slave_I2C_Mode = MD_RX;
        stcI2cCom.u32RecvDataWriteIndex = 0;
        stcI2cCom.u32TransDataWriteIndex = 0;
        memset(u8SlaveI2CRxBuf, 0, SLAVE_I2C_RX_BUF_LEN);
        memset(&PMessage, 0, sizeof(PanelMessage));
        IIC_ERROR_COUNT = 0;
    }
}

void hal::Wire_Init()
{
    Wire.begin();
    Wire.scanDevices();
    bq40z50_begin();
		mp2762a_begin(&batteryState);
}
