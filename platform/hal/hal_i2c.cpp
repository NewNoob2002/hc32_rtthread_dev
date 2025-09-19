#include "hal.h"
#include "crc.h"
#include "RingBuffer.h"
#include "nm_message.h"

uint8_t u8SlaveI2CRxBuf[SLAVE_I2C_RX_BUF_LEN];
uint8_t u8SlaveI2CTxBuf[SLAVE_I2C_TX_BUF_LEN];
unsigned int Slave_I2C_TX_Len = 0;

// stc_i2c_communication_t stcI2cCom;
// stc_i2c_com_mode_t Slave_I2C_Mode = MD_RX;
// i2c_slave_irqn_num_t i2c_slave_irqn_number;

void BufWrite(const uint8_t u8Data) {
  u8SlaveI2CRxBuf[stcI2cCom.u32RecvDataWriteIndex++] = u8Data;
  if (SLAVE_I2C_RX_BUF_LEN == stcI2cCom.u32RecvDataWriteIndex)
    stcI2cCom.u32RecvDataWriteIndex = 0;
}

uint8_t BufRead(void) {
  uint8_t temp = 0x55;
  if (stcI2cCom.u32TransDataWriteIndex < stcI2cCom.u32TxLen) {
    temp = u8SlaveI2CTxBuf[stcI2cCom.u32TransDataWriteIndex++];
  }
  if (stcI2cCom.u32TransDataWriteIndex >= stcI2cCom.u32TxLen) {
    /// temp = 0xFFU;
    stcI2cCom.u32TransDataWriteIndex = 0;
  }

  return temp;
}

void I2C_TEI_Callback(void) {
  if ((Set == I2C_GetStatus(I2C_UNIT, I2C_SR_TENDF)) &&
      (Reset == I2C_GetStatus(I2C_UNIT, I2C_SR_NACKF))) {
    I2C_WriteData(I2C_UNIT, BufRead());
  }
}

void I2C_RXI_Callback(void) {
  if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_RFULLF)) {
    BufWrite(I2C_ReadData(I2C_UNIT));
  }
}

void I2C_EEI_Callback(void) {
  /* If address interrupt occurred */
  if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_SLADDR0F)) {
    I2C_ClearStatus(I2C_UNIT, I2C_CLR_SLADDR0FCLR | I2C_CLR_NACKFCLR);

    if ((MD_TX == stcI2cCom.enMode) &&
        (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_TRA))) {
      /* Enable tx end interrupt function*/
      I2C_IntCmd(I2C_UNIT, I2C_CR2_TENDIE, Enable);
      /* Write the first data to DTR immediately */
      I2C_WriteData(I2C_UNIT, BufRead());

      /* Enable stop and NACK interrupt */
      I2C_IntCmd(I2C_UNIT, I2C_CR2_STOPIE | I2C_CR2_NACKIE, Enable);
    } else if ((MD_RX == stcI2cCom.enMode) &&
               (Reset == I2C_GetStatus(I2C_UNIT, I2C_SR_TRA))) {

      /* Enable stop and NACK interrupt */
      I2C_IntCmd(I2C_UNIT, I2C_CR2_STOPIE | I2C_CR2_NACKIE, Enable);
    }
  }
  /* If NACK interrupt occurred */
  else if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_NACKF)) {
    /* clear NACK flag*/
    I2C_ClearStatus(I2C_UNIT, I2C_CLR_NACKFCLR);
    /* Stop tx or rx process*/
    if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_TRA)) {
      /* Config tx end interrupt function disable*/
      I2C_IntCmd(I2C_UNIT, I2C_CR2_TENDIE, Disable);
      I2C_ClearStatus(I2C_UNIT, I2C_CLR_TENDFCLR);

      /* Read DRR register to release */
      I2C_ReadData(I2C_UNIT);
    } else {
      /* Config rx buffer full interrupt function disable */
      I2C_IntCmd(I2C_UNIT, I2C_CR2_RFULLIE, Disable);
    }
  }
  /* If stop interrupt occurred */
  else if (Set == I2C_GetStatus(I2C_UNIT, I2C_SR_STOPF)) {

    /* Disable all interrupt enable flag except SLADDR0IE*/
    I2C_IntCmd(I2C_UNIT, I2C_CR2_TENDIE | I2C_CR2_RFULLIE | I2C_CR2_STOPIE | I2C_CR2_NACKIE, Disable);
    /* Clear STOPF flag */
    I2C_ClearStatus(I2C_UNIT, I2C_CLR_STOPFCLR);
    I2C_Cmd(I2C_UNIT, Disable);
		if(Slave_I2C_Mode == MD_TX)
		{
			Slave_I2C_Mode = MD_RX;
			IIC_ERROR_COUNT = 0;
		}
    stcI2cCom.enComStatus = SLAVE_I2C_COM_IDLE;
  }
}

en_result_t I2C_Slave_Receive_IT(uint8_t *pu8RxData, const uint32_t u32Size) {
  en_result_t enRet = Ok;

  if (SLAVE_I2C_COM_IDLE == stcI2cCom.enComStatus) {
    stcI2cCom.enComStatus = SLAVE_I2C_COM_BUSY;

    // stcI2cCom.u32RecvDataWriteIndex=0;

    // stcI2cCom.u32DataIndex = 0u;
    stcI2cCom.enMode = MD_RX;
    stcI2cCom.u32RxLen = u32Size;
    stcI2cCom.pRxBuf = pu8RxData;

    I2C_Cmd(I2C_UNIT, Enable);
    /* Config slave address match interrupt function*/
    I2C_IntCmd(I2C_UNIT, I2C_CR2_SLADDR0EN | I2C_CR2_RFULLIE, Enable);
  } else {
    enRet = OperationInProgress;
  }

  return enRet;
}

en_result_t I2C_Slave_Transmit_IT(uint8_t *pu8TxData, const uint32_t u32Size) {
  en_result_t enRet = Ok;
  if (SLAVE_I2C_COM_IDLE == stcI2cCom.enComStatus) {
    stcI2cCom.enComStatus = SLAVE_I2C_COM_BUSY;

    //  stcI2cCom.u32DataIndex = 0u;
    //	stcI2cCom.u32TransDataWriteIndex=0;

    stcI2cCom.enMode = MD_TX;
    stcI2cCom.u32TxLen = u32Size;
    stcI2cCom.pTxBuf = pu8TxData;

    I2C_Cmd(I2C_UNIT, Enable);
    /* Config slave address match interrupt function*/
    I2C_IntCmd(I2C_UNIT, I2C_CR2_SLADDR0EN, Enable);
  } else {
    enRet = OperationInProgress;
  }

  return enRet;
}

void slave_i2c_get_irqn()
{
	irqn_aa_get(i2c_slave_irqn_number.irqn_event_error, "slave_i2c_event_error");
	irqn_aa_get(i2c_slave_irqn_number.irqn_rx_full, "slave_i2c_rx_full");
	irqn_aa_get(i2c_slave_irqn_number.irqn_tx_cpt, "slave_i2c_tx_cpt");
}

en_result_t Slave_Initialize(void) {
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
  stcI2cInit.u32Baudrate = 400000ul;
  stcI2cInit.u32SclTime = 5U;
  const en_result_t enRet = I2C_Init(I2C_UNIT, &stcI2cInit, &fErr);

  if (Ok == enRet) {
    /* Set slave address*/
#ifdef I2C_10BITS_ADDRESS
    I2C_SlaveAdr0Config(I2C_UNIT, Enable, Adr10bit, SLAVE_ADDRESS);
#else
    I2C_SlaveAdr0Config(I2C_UNIT, Enable, Adr7bit, SLAVE_ADDRESS);
#endif
		stcIrqRegiConf.enIRQn = i2c_slave_irqn_number.irqn_event_error;
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

    stcIrqRegiConf.enIRQn = i2c_slave_irqn_number.irqn_rx_full;
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

    stcIrqRegiConf.enIRQn = i2c_slave_irqn_number.irqn_tx_cpt;
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
  }

  stcI2cCom.u32RecvDataWriteIndex = 0;
  stcI2cCom.u32TransDataWriteIndex = 0;
  memset(u8SlaveI2CRxBuf, 0, SLAVE_I2C_RX_BUF_LEN);
  memset(&PMessage, 0, sizeof(PanelMessage));

  return enRet;
}

void IIC_deal(void) {
  if ((SLAVE_I2C_COM_BUSY != stcI2cCom.enComStatus) && (MD_RX == Slave_I2C_Mode)) {
    I2C_Slave_Receive_IT(u8SlaveI2CRxBuf, SLAVE_I2C_RX_BUF_LEN);
  }
  Panel_DecodeNMMessage(&stcI2cCom, &PMessage);
  if (MD_TX == Slave_I2C_Mode) {
    I2C_Slave_Transmit_IT(u8SlaveI2CTxBuf, Slave_I2C_TX_Len);
//    Slave_I2C_Mode = MD_RX;
//    IIC_ERROR_COUNT = 0;
  }
}

void hal::Slave_I2c_Init()
{
    slave_i2c_get_irqn();
    if (Slave_Initialize() != Ok)
    {
        CORE_DEBUG_PRINTF("I2c_slave Init failed\n");
    }
}

void hal::Slave_I2c_Deal(void *e)
{
    IIC_deal();
	  if (IIC_ERROR_COUNT >= 5000)
    {
				ERROR_PRINTF("i2cSlave\n");
        Slave_Initialize();
        Slave_I2C_Mode = MD_RX;
        stcI2cCom.u32RecvDataWriteIndex = 0;
        stcI2cCom.u32TransDataWriteIndex = 0;
        memset(u8SlaveI2CRxBuf, 0, SLAVE_I2C_RX_BUF_LEN);
        memset(&PMessage, 0, sizeof(PanelMessage));
        IIC_ERROR_COUNT = 0;
    }
}

//void hal::Wire_Init()
//{
//    Wire.begin();
//    Wire.scanDevices();
//    bq40z50_begin();
//		mp2762a_begin(&batteryState);
//}
