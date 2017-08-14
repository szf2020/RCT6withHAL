#ifndef _IIC_H_
#define _IIC_H_
#include "stm32f1xx_hal.h"

#define I2C_WR	        0		/* 写控制bit */
#define I2C_RD	        1		/* 读控制bit */
/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
/* AT24C01/02每页有8个字节 
 * AT24C04/08A/16A每页有16个字节 
 */
#define EEPROM_DEV_ADDR			   0xA0	    	/* 24xx02的设备地址 */
#define EEPROM_PAGE_SIZE		      8			  /* 24xx02的页面大小 */
#define EEPROM_SIZE				      256			  /* 24xx02总容量 */

#define IIC_SCL_HIGH()                      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET)    // 输出高电平
#define IIC_SCL_LOW()                       HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET)  // 输出低电平
#define IIC_SDA_HIGH()                      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET)    // 输出高电平
#define IIC_SDA_LOW()                       HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET)  // 输出低电平
#define IIC_SDA_READ()                      HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)

ERROR_STUS initIic(void);
uint8_t eepromReadBytes(uint8_t *ReadBuf, uint16_t Address, uint16_t Size);
uint8_t eepromWriteBytes(uint8_t *WriteBuf, uint16_t Address, uint16_t Size);
void iicTest(void);
#endif
