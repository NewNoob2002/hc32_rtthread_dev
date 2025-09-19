#ifndef MCU_DEFINE_H
#define MCU_DEFINE_H

#include <stdint.h>
#include <stdbool.h>
#include "hc32_ddl.h"

#define PANEL_MSG_MAX_LEN 1024

/* Define Write and read data length for the example */
#define SLAVE_I2C_RX_BUF_LEN 1024
#define SLAVE_I2C_TX_BUF_LEN 512

typedef enum key_action_t
{
	KEY_ACTION_NONE = 0,
	KEY_ACTION_SINGLE_CLICK, // 单击
	KEY_ACTION_DOUBLE_CLICK, // 双击
	KEY_ACTION_LONG_PRESS,	 // 长按
	KEY_ACTION_LONG_PRESS_REPEAT,
} key_action_t;

typedef enum chargeStatus_t
{
	notCharge = 0,
	normalCharge,
	fastCharge,
} chargeStatus_t;

typedef struct present_device
{
	bool i2c;
	bool bq40z50;
	bool mp2762;
} present_device;

typedef struct online_device
{
	bool i2c;
	bool bq40z50;
	bool mp2762;
} online_device;

typedef enum tag_work_mode_t
{
	rover_mode = 0,
	base_mode,
	single_mode,
	autobase_mode
} work_mode_t;

//typedef enum tag_turn_on_off_t
//{
//	interface_off = 0,
//	interface_on,
//} turn_on_off_t;

typedef struct tag_SystemInfo_t
{
    // 8-byte members
    double coordinate_lon;
    double coordinate_lat;
    double coordinate_alt;

    // 4-byte members
    float record_leftspace;
    uint8_t gprs_status;
    uint8_t NtripServer_status;
    uint8_t NtripClient_status;
    uint8_t radio_status;
    uint8_t wifi_status;
    work_mode_t work_mode;
    uint8_t NtripServer_IP[4];
    uint8_t NtripClient_IP[4];
    char wifi_ip[4];

    // Arrays
    char record_name[16]; //ascii
    uint8_t NtripServer_Mountpoint[32];
    uint8_t NtripClient_Mountpoint[32];
    char wifi_ssid[16];

    // 1-byte members
    unsigned char sw_reset_flag;
    unsigned char power_off_flag;
		bool isCharging;
    unsigned char record_status; // 0-off, 1-on
    uint8_t record_type;  //1-xyz 2-Rinex3.02
    uint8_t record_interval; // 0x00-15min, 0x01-60min, 0x02-120min, 0x04-240min, 0x18- 24hour
    unsigned char record_op;
    uint8_t coordinate_status;  // 0- none, 1-single, 4-fixed, 5- float, 7-fixedcoordinate
    uint8_t satellite_number_used;
    uint8_t satellite_number_track;
    uint8_t radio_mode;
    uint8_t radio_protocol;
    uint8_t radio_channel;
    uint8_t wifi_mode;
    uint8_t ExternalPower;
    uint8_t mcu_operation_flag;
    uint8_t linux_operation_flag;
		bool GuiNeedUpdate;
    bool sync_flag;
} SystemInfo_t;

typedef enum
{
	MD_TX = 0U,
	MD_RX = 1U,
	MD_NONE = 2U,
} stc_i2c_com_mode_t;

/**
 * @brief I2c communication status enum
 */
typedef enum
{
	SLAVE_I2C_COM_BUSY = 0U,
	SLAVE_I2C_COM_IDLE = 1U,
} stc_i2c_com_status_t;

/**
 * @brief I2c communication structure
 */
typedef struct
{
	stc_i2c_com_mode_t enMode; /*!< I2C communication mode*/
	uint32_t u32RxLen;		   /*!< I2C communication data length*/
	uint32_t u32TxLen;
	uint8_t *pRxBuf; /*!< I2C communication data buffer pointer*/
	uint8_t *pTxBuf;
	volatile uint32_t u32RecvDataWriteIndex; /*!< I2C communication data transfer index*/
	volatile uint32_t u32RecvDataReadIndex;
	volatile uint32_t u32TransDataWriteIndex;
	volatile uint32_t u32TransDataReadIndex;
	volatile stc_i2c_com_status_t enComStatus; /*!< I2C communication status*/
} stc_i2c_communication_t;

typedef struct i2c_slave_irqn_num
{
	IRQn_Type irqn_event_error;
	IRQn_Type irqn_rx_full;
	IRQn_Type irqn_tx_cpt;
}i2c_slave_irqn_num_t;

typedef struct tag_PanelMessage_s
{
	int w_index;
	unsigned char syn_head[4];
	unsigned int msg_len;
	unsigned char msg[PANEL_MSG_MAX_LEN + 1];
	unsigned char syn_state;
	unsigned char available;
} PanelMessage, *pPanelMessage;

typedef struct BatteryState
{
	uint16_t Percent;
	uint16_t Temp;
	uint16_t Voltage;
	chargeStatus_t chargeStatus;
	bool charge_time_record;
	uint32_t charge_start_time;
} BatteryState, *pBatteryState;

typedef struct Power
{
	uint32_t BatteryLastHandleTime;
	uint32_t ChargerLastHandleTime;
	uint16_t PowerKey_PressCount;
	stc_rmu_rstcause_t cause;
	bool LinuxPowerOff;
	bool LowBatteryPowerOff;
	bool PowerOffReq;
	bool PowerOFF_Ensure;
	
	bool mcu_powerOn_Done;
	
	bool Force_ShutDown_Powerkey;
	bool Force_ShutDown_Funckey;
} Power_Monitor_t;

typedef struct ledState
{
	uint32_t lastToggleTime;
	uint32_t currentRate;
} ledState;

extern SystemInfo_t systemInfo;
extern BatteryState batteryState;
extern Power_Monitor_t Power_Monitor;
extern online_device online_devices;

extern PanelMessage PMessage;

extern uint8_t u8SlaveI2CRxBuf[SLAVE_I2C_RX_BUF_LEN];
extern uint8_t u8SlaveI2CTxBuf[SLAVE_I2C_TX_BUF_LEN];
extern stc_i2c_communication_t stcI2cCom;
extern i2c_slave_irqn_num_t i2c_slave_irqn_number;
extern stc_i2c_com_mode_t Slave_I2C_Mode;
extern unsigned int Slave_I2C_TX_Len;

extern volatile uint32_t IIC_ERROR_COUNT;
extern volatile uint32_t LOW_POWER_COUNT;
#endif