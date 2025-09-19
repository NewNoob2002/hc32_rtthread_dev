#include "hal.h"
#include "crc.h"
#include "RingBuffer.h"

// i2c slave rx / tx buffers (unboxed from config)
RingBuffer<uint8_t> *i2cSlave_rxBuffer = nullptr;
RingBuffer<uint8_t> *i2cSlave_txBuffer = nullptr;

i2c_slave_irqn_num_t i2cSlave_irqn_number;
stc_i2c_communication_t stcI2cCom;
stc_i2c_com_mode_t Slave_I2C_Mode = MD_RX;

static void I2C_TEI_Callback(void)
{
    if ((Set == I2C_GetStatus(I2C_UNIT, I2C_SR_TENDF)) &&
        (Reset == I2C_GetStatus(I2C_UNIT, I2C_SR_NACKF)))
    {
        uint8_t data;
        if (i2cSlave_txBuffer->pop(data))
        {
            I2C_WriteData(I2C_UNIT, data);
        }
    }
}

static void I2C_RXI_Callback(void)
{
    if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_RFULLF))
    {
		INFO_PRINTF("RXI_Callback");
        uint8_t data = I2C_ReadData(I2C_UNIT);
        if (!i2cSlave_rxBuffer->push(data, true))
        {
            ERROR_PRINTF("i2cSlave_rxBuffer push failed");
        }
    }
}

static void I2C_EEI_Callback(void)
{
	INFO_PRINTF("EEI_Callback");
    /* If address interrupt occurred */
    if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_SLADDR0F))
    {
        INFO_PRINTF("Address Match");
        I2C_ClearStatus(I2C_UNIT, I2C_CLR_SLADDR0FCLR | I2C_CLR_NACKFCLR);

        if ((MD_TX == stcI2cCom.enMode) &&
            (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_TRA)))
        {
            INFO_PRINTF("i2cSlave TX");
            /* Enable tx end interrupt function*/
            I2C_IntCmd(I2C_UNIT, I2C_CR2_TENDIE, Enable);
            /* Write the first data to DTR immediately */
            uint8_t data;
            if (i2cSlave_txBuffer->pop(data))
            {
                I2C_WriteData(I2C_UNIT, data);
            }

            /* Enable stop and NACK interrupt */
            I2C_IntCmd(I2C_UNIT, I2C_CR2_STOPIE | I2C_CR2_NACKIE, Enable);
        }
        else if ((Reset == I2C_GetStatus(I2C_UNIT, I2C_SR_TRA)))
        {
            INFO_PRINTF("i2cSlave RX");
            /* Enable stop and NACK interrupt */
            I2C_IntCmd(I2C_UNIT, I2C_CR2_RFULLIE | I2C_CR2_STOPIE | I2C_CR2_NACKIE, Enable);
        }
    }
    /* If NACK interrupt occurred */
    else if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_NACKF))
    {
        INFO_PRINTF("NACK interrupt occurred");
        /* clear NACK flag*/
        I2C_ClearStatus(I2C_UNIT, I2C_CLR_NACKFCLR);
        /* Stop tx or rx process*/
        if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_TRA))
        {
            /* Config tx end interrupt function disable*/
            I2C_IntCmd(I2C_UNIT, I2C_CR2_TENDIE, Disable);
            I2C_ClearStatus(I2C_UNIT, I2C_CLR_TENDFCLR);

            /* Read DRR register to release */
            I2C_ReadData(I2C_UNIT);
        }
        else
        {
            /* Config rx buffer full interrupt function disable */
            I2C_IntCmd(I2C_UNIT, I2C_CR2_RFULLIE, Disable);
        }
    }
    /* If stop interrupt occurred */
    else if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_STOPF))
    {
        INFO_PRINTF("STOP interrupt occurred");
        /* Disable all interrupt enable flag except SLADDR0IE*/
        I2C_IntCmd(I2C_UNIT, I2C_CR2_TENDIE |  I2C_CR2_STOPIE | I2C_CR2_NACKIE, Disable);
        /* Clear STOPF flag */
        I2C_ClearStatus(I2C_UNIT, I2C_CLR_STOPFCLR);
        //I2C_Cmd(I2C_UNIT, Disable);
        if (Slave_I2C_Mode == MD_TX)
        {
            Slave_I2C_Mode = MD_RX;
            IIC_ERROR_COUNT = 0;
        }
        stcI2cCom.enComStatus = SLAVE_I2C_COM_IDLE;
    }
}

static void i2cSlave_get_irqn()
{
    irqn_aa_get(i2cSlave_irqn_number.irqn_event_error, "slave_i2c_event_error");
    irqn_aa_get(i2cSlave_irqn_number.irqn_rx_full, "slave_i2c_rx_full");
    irqn_aa_get(i2cSlave_irqn_number.irqn_tx_cpt, "slave_i2c_tx_cpt");
}

en_result_t i2cSlave_Initialize()
{
    stc_i2c_init_t stcI2cInit;
    stc_irq_regi_conf_t stcIrqRegiConf;
    float32_t fErr;

    /* Initialize I2C port*/
    PORT_SetFunc(I2C_SCL_PORT, I2C_SCL_PIN, I2C_GPIO_SCL_FUNC, Disable);
    PORT_SetFunc(I2C_SDA_PORT, I2C_SDA_PIN, I2C_GPIO_SDA_FUNC, Disable);
    /* Enable I2C Peripheral*/
    PWC_Fcg1PeriphClockCmd(I2C_FCG_USE, Enable);
    /* Initialize I2C peripheral and enable function*/

    I2C_DeInit(I2C_UNIT);

    stcI2cCom.enComStatus = SLAVE_I2C_COM_IDLE;

    MEM_ZERO_STRUCT(stcI2cInit);
    stcI2cInit.u32ClockDiv = I2C_CLK_DIV2;
    stcI2cInit.u32Baudrate = I2C_BAUDRATE;
    stcI2cInit.u32SclTime = 5U;
    const en_result_t enRet = I2C_Init(I2C_UNIT, &stcI2cInit, &fErr);
    if (enRet != Ok)
    {
        ERROR_PRINTF("I2C_Init failed, error:%f, ret = %d", fErr, enRet);
        return enRet;
    }
		i2cSlave_get_irqn();
    I2C_SlaveAdr0Config(I2C_UNIT, Enable, Adr7bit, SLAVE_ADDRESS);
    stcIrqRegiConf.enIRQn = i2cSlave_irqn_number.irqn_event_error;
    /* Select I2C Error or Event interrupt function */
    stcIrqRegiConf.enIntSrc = I2C_INT_EEI_DEF;
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &I2C_EEI_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_00);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    stcIrqRegiConf.enIRQn = i2cSlave_irqn_number.irqn_rx_full;
    /* Select I2C receive full interrupt function */
    stcIrqRegiConf.enIntSrc = I2C_INT_RXI_DEF;
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &I2C_RXI_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_00); // DDL_IRQ_PRIORITY_DEFAULT
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    stcIrqRegiConf.enIRQn = i2cSlave_irqn_number.irqn_tx_cpt;
    /* Select I2C TX end interrupt function */
    stcIrqRegiConf.enIntSrc = I2C_INT_TEI_DEF;
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &I2C_TEI_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_00); // DDL_IRQ_PRIORITY_DEFAULT
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    if(i2cSlave_rxBuffer == nullptr)
    {
    i2cSlave_rxBuffer = new RingBuffer<uint8_t>(512);
    }
		RT_ASSERT(i2cSlave_rxBuffer != nullptr);
    if(i2cSlave_txBuffer == nullptr)
    {
        i2cSlave_txBuffer = new RingBuffer<uint8_t>(256);
    }
		RT_ASSERT(i2cSlave_txBuffer != nullptr);
    return Ok;
}

en_result_t I2C_Slave_Receive_IT() {
    en_result_t enRet = Ok;
  
    if (SLAVE_I2C_COM_IDLE == stcI2cCom.enComStatus) {
      stcI2cCom.enComStatus = SLAVE_I2C_COM_BUSY;
      stcI2cCom.enMode = MD_RX;
  
      I2C_Cmd(I2C_UNIT, Enable);
      /* Config slave address match interrupt function*/
      I2C_IntCmd(I2C_UNIT, I2C_CR2_SLADDR0EN | I2C_CR2_RFULLIE, Enable);
    } else {
      enRet = OperationInProgress;
    }
  
    return enRet;
}

size_t i2cSlave_rxbuf_available()
{
    return i2cSlave_rxBuffer->count();
}

int i2cSlave_rxbuf_read()
{
    uint8_t data;
    if(i2cSlave_rxBuffer->pop(data))
    {
        return data;
    }
    return -1;
}

int i2cSlave_rxbuf_readBytes(uint8_t *data, size_t size)
{
    int readBytes = 0;
    while(size > 0)
    {
        uint8_t temp;
        if(i2cSlave_rxBuffer->pop(temp))
        {
            *data++ = temp;
            size--;
            readBytes++;
        }
        else
        {
            break;
        }
    }
    return readBytes;
}