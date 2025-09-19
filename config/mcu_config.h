#ifndef MCU_CONFIG_H
#define MCU_CONFIG_H

#include "mcu_define.h"
#include "system.h"

#define FIRMWARE_NAME "X1_5WRadio_Panel"
#define SOFTWARE_VERSION "1.0.0"
#define SOFTWARE_BUILD_DATE __DATE__
#define SOFTWARE_BUILD_TIME __TIME__
#define HARDWARE_VERSION "1.0"

/*Function Key*/
#define FUNCTION_KEY_PIN PA15
/*power_control_pin*/
#define MCU_ON_OFF_BATTERY_PIN PB3
/*power_key_pin*/
#define POWER_KEY_PIN PA0

/*charger_control*/
#define CHARGER_ENABLE_PIN 		PA4
#define CHARGER_CTRL_PIN 			PB7
#define CHARGER_CTRL_FAST_PIN PB6
#define CHARGER_SWITCH_PIN 		PB10

#define CHARGER_ADC_DETECT_PIN PA1

#define USB_SWITCH_PIN PB8

/*watchdog_pin*/
#define WATCHDOG_PIN PA6

/*Status LED*/
#define POWER_LED_PIN PC13
#define CHARGE_LED_PIN PH2
#define FUNCTION_LED_PIN PB5

/*SlaveI2c*/
#define I2C_UNIT (M4_I2C1)
/* Define slave device address, for example */
#define SLAVE_ADDRESS (0x11u)
// #define I2C_10BITS_ADDRESS              (1u)

/* Define port and pin for SDA and SCL */
#define I2C_SCL_PORT (PortA)
#define I2C_SCL_PIN  (Pin03)
#define I2C_SDA_PORT (PortA)
#define I2C_SDA_PIN  (Pin02)
#define I2C_GPIO_SCL_FUNC (Func_I2c1_Scl)
#define I2C_GPIO_SDA_FUNC (Func_I2c1_Sda)

#define I2C_INT_EEI_DEF (INT_I2C1_EEI)																				
#define I2C_INT_RXI_DEF (INT_I2C1_RXI)
#define I2C_INT_TXI_DEF (INT_I2C1_TXI)
#define I2C_INT_TEI_DEF (INT_I2C1_TEI)

#define I2C_FCG_USE (PWC_FCG1_PERIPH_I2C1)
/* Define i2c baud rate */
#define I2C_BAUDRATE (400000ul)

#define TIMEOUT (0x40000UL)
#endif