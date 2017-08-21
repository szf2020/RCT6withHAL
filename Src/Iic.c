#include "Iic.h"
/*
funName	:IicDelay
input		:
output	:
describe:iic ��ʱ����
remark	:
*/
static void iicDelay(void)
{
	uint8_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
		CPU��Ƶ72MHzʱ�����ڲ�Flash����, MDK���̲��Ż�
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
        
    IAR���̱���Ч�ʸߣ���������Ϊ7
	*/
	for (i = 0; i < 10; i++);
}
/*
funName	:iicStart
input		:
output	:
describe:iic ��ʼ�ź�
remark	:
*/
void iicStart(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
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
describe:iic ��ֹ�ź�
remark	:
*/
void iicStop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	IIC_SDA_LOW();
	IIC_SCL_HIGH();
	iicDelay();
	IIC_SDA_HIGH();
}
/*
funName	:iicSendByte
input		:
output	:
describe:iic �����ֽ�
remark	:
*/
void iicSendByte(uint8_t Byte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
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
			IIC_SDA_HIGH(); // �ͷ�����
		}
		Byte <<= 1;	/* ����һ��bit */
		iicDelay();
	}
}
/*
funName	:iicReadByte
input		:
output	:
describe:iic ��ȡ�ֽ�
remark	:
*/
uint8_t iicReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
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
output	:����0��ʾ��ȷӦ��1��ʾ��������Ӧ
describe:iic �ȴ�Ӧ��
remark	:
*/
uint8_t iicWaitAck(void)
{
	uint8_t re;

	IIC_SDA_HIGH();	/* CPU�ͷ�SDA���� */
	iicDelay();
	IIC_SCL_HIGH();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	iicDelay();
	if (IIC_SDA_READ())	/* CPU��ȡSDA����״̬ */
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
describe:iic Ӧ��
remark	:CPU����һ��ACK�ź�
*/
void iicAck(void)
{
	IIC_SDA_LOW();	/* CPU����SDA = 0 */
	iicDelay();
	IIC_SCL_HIGH();	/* CPU����1��ʱ�� */
	iicDelay();
	IIC_SCL_LOW();
	iicDelay();
	IIC_SDA_HIGH();	/* CPU�ͷ�SDA���� */
}
/*
funName	:iicNAck
input		:
output	:
describe:iic NACK
remark	:CPU����1��NACK�ź�
*/
void iicNAck(void)
{
	IIC_SDA_HIGH();	/* CPU����SDA = 1 */
	iicDelay();
	IIC_SCL_HIGH();	/* CPU����1��ʱ�� */
	iicDelay();
	IIC_SCL_LOW();
	iicDelay();	
}
/*
funName	:checkDevice
input		:
output	:����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽iic�豸
describe:iic�豸���
remark	:
*/
uint8_t checkDevice(uint8_t address){
	uint8_t ucAck;
	iicStart();		/* ���������ź� */
	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	iicSendByte(EEPROM_DEV_ADDR | I2C_WR);
	ucAck = iicWaitAck();	/* ����豸��ACKӦ�� */
	iicStop();			/* ����ֹͣ�ź� */
	return ucAck;
}
/*
funName	:initIic
input		:
output	:ERROR_STUS
describe:iic ��ʼ�� 
remark	:
*/
ERROR_STUS initIic(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//SCL PC12,SDA PC11
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	iicStop();
	
	if(checkDevice(EEPROM_DEV_ADDR) == 0){
		return E_OK;
	}else{
		iicStop();/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		return E_IIC;
	}
}
/*
funName	:eepromReadBytes
input		:
*ReadBuf:����iic��ȡ�������ֽ�
Address:Ҫ��ȡ�ĵ�ַ
Size:��ȡ�ֽڵĴ�С
output	:1:�ɹ�0:ʧ��
describe:eeprom ��ȡ����
remark	:
*/
uint8_t eepromReadBytes(uint8_t *ReadBuf, uint16_t Address, uint16_t Size)
{
	uint16_t i;
	
	/* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */
	
	/* ��1��������I2C���������ź� */
	iicStart();	
  
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	iicSendByte(EEPROM_DEV_ADDR | I2C_WR);	/* �˴���дָ�� */	
  
	/* ��3�����ȴ�ACK */
	if (iicWaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
  
	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	iicSendByte((uint8_t)Address);	
  
	/* ��5�����ȴ�ACK */
	if (iicWaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}	
	/* ��6������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ���� */
	iicStart();	
  
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	iicSendByte(EEPROM_DEV_ADDR | I2C_RD);	/* �˴��Ƕ�ָ�� */
	
	/* ��8��������ACK */
	if (iicWaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}	
	
	/* ��9����ѭ����ȡ���� */
	for (i = 0; i < Size; i++)
	{
		ReadBuf[i] = iicReadByte();	/* ��1���ֽ� */
		
		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != Size - 1)
		{
			iicAck();	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			iicNAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	/* ����I2C����ֹͣ�ź� */
	iicStop();
	return 1;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	iicStop();
	return 0;
}
/*
funName	:eepromReadBytes
input		:
*WriteBuf:д���豸������
Address:Ҫд�����ʼ��ַ
Size:д�����ݵĴ�С
output	:1:�ɹ�0:ʧ��
describe:eeprom д������
remark	:
*/
uint8_t eepromWriteBytes(uint8_t *WriteBuf, uint16_t Address, uint16_t Size)
{
	uint16_t i,m;
	uint16_t usAddr;
	
	/* 
	 * д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��
	 * ����24xx02��page size = 8
	 * �򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ûд1���ֽڣ������͵�ַ
	 * Ϊ���������д��Ч��: ����������page wirte������
	 */

	usAddr = Address;	
	for (i = 0; i < Size; i++)
	{
		/* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
		if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
		{
			/*���ڣ�������ֹͣ�źţ������ڲ�д������*/
			iicStop();
			
			/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms 			
				CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
			*/
			for (m = 0; m < 1000; m++)
			{				
				/* ��1��������I2C���������ź� */
				iicStart();
				
				/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
				iicSendByte(EEPROM_DEV_ADDR | I2C_WR);	/* �˴���дָ�� */
				
				/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
				if (iicWaitAck() == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	/* EEPROM����д��ʱ */
			}
		
			/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
			iicSendByte((uint8_t)usAddr);
			
			/* ��5�����ȴ�ACK */
			if (iicWaitAck() != 0)
			{
				goto cmd_fail;	/* EEPROM������Ӧ�� */
			}
		}
	
		/* ��6������ʼд������ */
		iicSendByte(WriteBuf[i]);
	
		/* ��7��������ACK */
		if (iicWaitAck() != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}

		usAddr++;	/* ��ַ��1 */		
	}
	
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	iicStop();
	return 1;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	iicStop();
	return 0;
}

void iicTest(void){
	uint16_t i;
	uint8_t I2c_Buf_Write[256]={0};
	uint8_t I2c_Buf_Read[256]={0};
//	printf("��⵽����EEPROM��AT24C02��оƬ\n");
//	printf("��д�������:\n");    
	for ( i=0; i<256; i++ ) //��仺��
	{
		I2c_Buf_Read[i]=0;      // ��ս��ջ�����
		I2c_Buf_Write[i] = i;   // Ϊ���ͻ������������
//		printf("0x%02X ", I2c_Buf_Write[i]);
//		if(i%16 == 15)    
//				printf("\n");
	 }
	 //��I2c_Buf_Write��˳�����������д��EERPOM�� 
	 eepromWriteBytes(I2c_Buf_Write, 0, 256);  
	 HAL_Delay(100);
	 
//	 printf("����������:\n");
	 //��EEPROM��������˳�򱣳ֵ�I2c_Buf_Read��  
	 eepromReadBytes(I2c_Buf_Read, 0, 256); 
	 for (i=0;i<256;i++)
	 {    
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
//			printf("0x%02X ", I2c_Buf_Read[i]);
//			printf("����:I2C EEPROMд������������ݲ�һ��\n\r");
			break;
		}
//		printf("0x%02X ", I2c_Buf_Read[i]);
//		if(i%16 == 15)    
//				printf("\n");
	}
	if(i==256)
	{
//		printf("EEPROM(AT24C02)��д���Գɹ�\n\r");
	}
}
