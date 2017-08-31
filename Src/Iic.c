#include "Iic.h"
/*
funName	:IicDelay
input		:
output	:
describe:iic 延时函数
remark	:
*/
static void iicDelay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
		CPU主频72MHz时，在内部Flash运行, MDK工程不优化
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
        
    IAR工程编译效率高，不能设置为7
	*/
	for (i = 0; i < 10; i++);
}
/*
funName	:iicStart
input		:
output	:
describe:iic 起始信号
remark	:
*/
void iicStart(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	IIC_SDA_HIGH();
	IIC_SCL_HIGH();
	iicDelay();
	IIC_SDA_LOW();
	iicDelay();
	IIC_SCL_LOW();
	iicDelay();
}
/*
funName	:iicStop
input		:
output	:
describe:iic 终止信号
remark	:
*/
void iicStop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	IIC_SDA_LOW();
	IIC_SCL_HIGH();
	iicDelay();
	IIC_SDA_HIGH();
}
/*
funName	:iicSendByte
input		:
output	:
describe:iic 发送字节
remark	:
*/
void iicSendByte(uint8_t Byte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (Byte & 0x80)
		{
			IIC_SDA_HIGH();
		}
		else
		{
			IIC_SDA_LOW();
		}
		iicDelay();
		IIC_SCL_HIGH();
		iicDelay();	
		IIC_SCL_LOW();
		if (i == 7)
		{
			IIC_SDA_HIGH(); // 释放总线
		}
		Byte <<= 1;	/* 左移一个bit */
		iicDelay();
	}
}
/*
funName	:iicReadByte
input		:
output	:
describe:iic 读取字节
remark	:
*/
uint8_t iicReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		IIC_SCL_HIGH();
		iicDelay();
		if (IIC_SDA_READ())
		{
			value++;
		}
		IIC_SCL_LOW();
		iicDelay();
	}
	return value;
}
/*
funName	:iicWaitAck
input		:
output	:返回0表示正确应答，1表示无器件响应
describe:iic 等待应答
remark	:
*/
uint8_t iicWaitAck(void)
{
	uint8_t re;

	IIC_SDA_HIGH();	/* CPU释放SDA总线 */
	iicDelay();
	IIC_SCL_HIGH();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	iicDelay();
	if (IIC_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	IIC_SCL_LOW();
	iicDelay();
	return re;
}
/*
funName	:iicAck
input		:
output	:
describe:iic 应答
remark	:CPU产生一个ACK信号
*/
void iicAck(void)
{
	IIC_SDA_LOW();	/* CPU驱动SDA = 0 */
	iicDelay();
	IIC_SCL_HIGH();	/* CPU产生1个时钟 */
	iicDelay();
	IIC_SCL_LOW();
	iicDelay();
	IIC_SDA_HIGH();	/* CPU释放SDA总线 */
}
/*
funName	:iicNAck
input		:
output	:
describe:iic NACK
remark	:CPU产生1个NACK信号
*/
void iicNAck(void)
{
	IIC_SDA_HIGH();	/* CPU驱动SDA = 1 */
	iicDelay();
	IIC_SCL_HIGH();	/* CPU产生1个时钟 */
	iicDelay();
	IIC_SCL_LOW();
	iicDelay();	
}
/*
funName	:checkDevice
input		:
output	:返回值 0 表示正确， 返回1表示未探测到iic设备
describe:iic设备检测
remark	:
*/
uint8_t checkDevice(uint8_t address){
	uint8_t ucAck;
	iicStart();		/* 发送启动信号 */
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	iicSendByte(EEPROM_DEV_ADDR | I2C_WR);
	ucAck = iicWaitAck();	/* 检测设备的ACK应答 */
	iicStop();			/* 发送停止信号 */
	return ucAck;
}
/*
funName	:initIic
input		:
output	:ERROR_STUS
describe:iic 初始化 
remark	:
*/
ERROR_STUS initIic(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//SCL PC12,SDA PC11
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	iicStop();
	
	if(checkDevice(EEPROM_DEV_ADDR) == 0){
		return E_OK;
	}else{
		iicStop();/* 失败后，切记发送I2C总线停止信号 */
		return E_IIC;
	}
}
/*
funName	:eepromReadBytes
input		:
*ReadBuf:接收iic读取的所有字节
Address:要读取的地址
Size:读取字节的大小
output	:1:成功0:失败
describe:eeprom 读取内容
remark	:
*/
uint8_t eepromReadBytes(uint8_t *ReadBuf, uint16_t Address, uint16_t Size)
{
	uint16_t i;
	
	/* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */
	
	/* 第1步：发起I2C总线启动信号 */
	iicStart();	
  
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	iicSendByte(EEPROM_DEV_ADDR | I2C_WR);	/* 此处是写指令 */	
  
	/* 第3步：等待ACK */
	if (iicWaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}
  
	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	iicSendByte((uint8_t)Address);	
  
	/* 第5步：等待ACK */
	if (iicWaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}	
	/* 第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */
	iicStart();	
  
	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	iicSendByte(EEPROM_DEV_ADDR | I2C_RD);	/* 此处是读指令 */
	
	/* 第8步：发送ACK */
	if (iicWaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}	
	
	/* 第9步：循环读取数据 */
	for (i = 0; i < Size; i++)
	{
		ReadBuf[i] = iicReadByte();	/* 读1个字节 */
		
		/* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
		if (i != Size - 1)
		{
			iicAck();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
		}
		else
		{
			iicNAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
		}
	}
	/* 发送I2C总线停止信号 */
	iicStop();
	return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	iicStop();
	return 0;
}
/*
funName	:eepromReadBytes
input		:
*WriteBuf:写入设备的内容
Address:要写入的起始地址
Size:写入内容的大小
output	:1:成功0:失败
describe:eeprom 写入内容
remark	:
*/
uint8_t eepromWriteBytes(uint8_t *WriteBuf, uint16_t Address, uint16_t Size)
{
	uint16_t i,m;
	uint16_t usAddr;
	
	/* 
	 * 写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
	 * 对于24xx02，page size = 8
	 * 简单的处理方法为：按字节写操作模式，没写1个字节，都发送地址
	 * 为了提高连续写的效率: 本函数采用page wirte操作。
	 */

	usAddr = Address;	
	for (i = 0; i < Size; i++)
	{
		/* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
		if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
		{
			/*　第０步：发停止信号，启动内部写操作　*/
			iicStop();
			
			/* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms 			
				CLK频率为200KHz时，查询次数为30次左右
			*/
			for (m = 0; m < 1000; m++)
			{				
				/* 第1步：发起I2C总线启动信号 */
				iicStart();
				
				/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
				iicSendByte(EEPROM_DEV_ADDR | I2C_WR);	/* 此处是写指令 */
				
				/* 第3步：发送一个时钟，判断器件是否正确应答 */
				if (iicWaitAck() == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	/* EEPROM器件写超时 */
			}
		
			/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
			iicSendByte((uint8_t)usAddr);
			
			/* 第5步：等待ACK */
			if (iicWaitAck() != 0)
			{
				goto cmd_fail;	/* EEPROM器件无应答 */
			}
		}
	
		/* 第6步：开始写入数据 */
		iicSendByte(WriteBuf[i]);
	
		/* 第7步：发送ACK */
		if (iicWaitAck() != 0)
		{
			goto cmd_fail;	/* EEPROM器件无应答 */
		}

		usAddr++;	/* 地址增1 */		
	}
	
	/* 命令执行成功，发送I2C总线停止信号 */
	iicStop();
	return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	iicStop();
	return 0;
}

void iicTest(void){
	uint16_t i;
	uint8_t I2c_Buf_Write[256]={0};
	uint8_t I2c_Buf_Read[256]={0};
	printf("检测到板载EEPROM（AT24C02）芯片\n");
	printf("待写入的数据:\n");    
	for ( i=0; i<256; i++ ) //填充缓冲
	{
		I2c_Buf_Read[i]=0;      // 清空接收缓冲区
		I2c_Buf_Write[i] = i;   // 为发送缓冲区填充数据
		printf("0x%02X ", I2c_Buf_Write[i]);
		if(i%16 == 15)    
				printf("\n");
	 }
	 //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
	 eepromWriteBytes(I2c_Buf_Write, 0, 256);  
	 HAL_Delay(100);
	 
	 printf("读出的数据:\n");
	 //将EEPROM读出数据顺序保持到I2c_Buf_Read中  
	 eepromReadBytes(I2c_Buf_Read, 0, 256); 
	 for (i=0;i<256;i++)
	 {    
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			printf("错误:I2C EEPROM写入与读出的数据不一致\n\r");
			break;
		}
		printf("0x%02X ", I2c_Buf_Read[i]);
		if(i%16 == 15)    
				printf("\n");
	}
	if(i==256)
	{
		printf("EEPROM(AT24C02)读写测试成功\n\r");
	}
}
