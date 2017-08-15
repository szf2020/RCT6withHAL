#include "Spi.h"
SPI_HandleTypeDef hspiflash;



/*
funName	:initSpi
input		:*spix
output	:
describe:��ʼ��spix
remark	:
*/
void initSpix(SPI_TypeDef *spix){
  hspiflash.Instance = spix;
  hspiflash.Init.Mode = SPI_MODE_MASTER;
  hspiflash.Init.Direction = SPI_DIRECTION_2LINES;
  hspiflash.Init.DataSize = SPI_DATASIZE_8BIT;
  hspiflash.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspiflash.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspiflash.Init.NSS = SPI_NSS_SOFT;
  hspiflash.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspiflash.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspiflash.Init.TIMode = SPI_TIMODE_DISABLE;
  hspiflash.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspiflash.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspiflash);
}
/*
funName	:HAL_SPI_MspInit
input		:* hspi
output	:
describe:��д��ʼ��spigpio
remark	:
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance==SPI1)
  { 
    /**SPI1 GPIO Configuration    
    PA2     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}
/*
funName	:spiFlashSendByte
input		:byte
output	:uint8_t
describe:spi�з��صķ���һ���ֽڣ��������ʧ�ܷ���0xff
remark	:
*/
uint8_t spiFlashSendByte(uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(&hspiflash,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read; 
}
/*
funName	:spiFlashWriteEnable
input		:
output	:
describe:spi дʹ��
remark	:
*/
void spiFlashWriteEnable(void)
{
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();

  /* �������дʹ�� */
  spiFlashSendByte(W25X_WriteEnable);

  /* ���ô���Flash��CS�ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
}
/*
funName	:spiFlashWaitForWriteEnd
input		:
output	:
describe:spi �ȴ�����д�����
remark	:
*/
void spiFlashWaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Read Status Register" instruction */
  spiFlashSendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = spiFlashSendByte(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE();
}
/*
funName	:spiFlashSectorErase
input		:
output	:
describe:spi ��������
remark	:
*/
void spiFlashSectorErase(uint32_t SectorAddr)
{
  /* ����FLASHдʹ������ */
  spiFlashWriteEnable();
  spiFlashWaitForWriteEnd();
  /* �������� */
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();
  /* ������������ָ��*/
  spiFlashSendByte(W25X_SectorErase);
  /*���Ͳ���������ַ�ĸ�λ*/
  spiFlashSendByte((SectorAddr & 0xFF0000) >> 16);
  /* ���Ͳ���������ַ����λ */
  spiFlashSendByte((SectorAddr & 0xFF00) >> 8);
  /* ���Ͳ���������ַ�ĵ�λ */
  spiFlashSendByte(SectorAddr & 0xFF);
  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
  /* �ȴ��������*/
  spiFlashWaitForWriteEnd();
}
/*
funName	:spiFlashBulkErase
input		:
output	:
describe:spi ����оƬ
remark	:
*/
void spiFlashBulkErase(void)
{
 /* ����FLASHдʹ������ */
  spiFlashWriteEnable();

  /* ��Ƭ���� Erase */
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();
  /* ������Ƭ����ָ��*/
  spiFlashSendByte(W25X_ChipErase);
  /* ���ô���FLASH: CS�ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();

  /* �ȴ��������*/
  spiFlashWaitForWriteEnd();
}
/*
funName	:spiFlashPageWrite
input		:
pBuffer����д�����ݵ�ָ��
WriteAddr��д���ַ
NumByteToWrite��д�����ݳ���
output	:
describe:spi ҳд�룬������FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
remark	: ����Flashÿҳ��СΪ256���ֽ�
*/
void spiFlashPageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* ����FLASHдʹ������ */
  spiFlashWriteEnable();

   /* Ѱ�Ҵ���FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();
  /* д��дָ��*/
  spiFlashSendByte(W25X_PageProgram);
  /*����д��ַ�ĸ�λ*/
  spiFlashSendByte((WriteAddr & 0xFF0000) >> 16);
  /*����д��ַ����λ*/
  spiFlashSendByte((WriteAddr & 0xFF00) >> 8);
  /*����д��ַ�ĵ�λ*/
  spiFlashSendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("Err: SPI_FLASH_PageWrite too large!\n");
  }

  /* д������*/
  while (NumByteToWrite--)
  {
     /* ���͵�ǰҪд����ֽ����� */
    spiFlashSendByte(*pBuffer);
     /* ָ����һ�ֽ����� */
    pBuffer++;
  }

  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();

  /* �ȴ�д�����*/
  spiFlashWaitForWriteEnd();
}
/*
funName	:spiFlashBufferWrite
input		:
pBuffer����д�����ݵ�ָ��
WriteAddr��д���ַ
NumByteToWrite��д�����ݳ���
output	:
describe:������FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
remark	: �ú���������������д�����ݳ���
*/
void spiFlashBufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* ����ַ�� SPI_FLASH_PageSize ����  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      spiFlashPageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        spiFlashPageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      spiFlashPageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* ����ַ�� SPI_FLASH_PageSize ������ */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        spiFlashPageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        spiFlashPageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        spiFlashPageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      spiFlashPageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        spiFlashPageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        spiFlashPageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}
/*
funName	:spiFlashBufferRead
input		:
pBuffer����Ŷ�ȡ�����ݵ�ָ��
ReadAddr����ȡ����Ŀ���ַ
NumByteToRead����ȡ���ݳ���
output	:
describe:��ȡflash����
remark	: �ú����������������ȡ���ݳ���
*/
void spiFlashBufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();

  /* ���� �� ָ�� */
  spiFlashSendByte(W25X_ReadData);

  /* ���� �� ��ַ��λ */
  spiFlashSendByte((ReadAddr & 0xFF0000) >> 16);
  /* ���� �� ��ַ��λ */
  spiFlashSendByte((ReadAddr& 0xFF00) >> 8);
  /* ���� �� ��ַ��λ */
  spiFlashSendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* ��ȡ���� */
  {
     /* ��ȡһ���ֽ�*/
    *pBuffer = spiFlashSendByte(Dummy_Byte);
    /* ָ����һ���ֽڻ����� */
    pBuffer++;
  }

  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
}
/*
funName	:spiFlashReadID
input		:
output	:
describe:��ȡflash�ͺ�ID
remark	:24���ֽ�
FLASH_ID      IC�ͺ�      �洢�ռ��С         
							0xEF3015      W25X16        2M byte
							0xEF4015	    W25Q16        4M byte
							0XEF4017      W25Q64        8M byte
							0XEF4018      W25Q128       16M byte
*/
uint32_t spiFlashReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();

  /* ���������ȡоƬ�ͺ�ID */
  spiFlashSendByte(W25X_JedecDeviceID);

  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp0 = spiFlashSendByte(Dummy_Byte);

  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp1 = spiFlashSendByte(Dummy_Byte);

  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp2 = spiFlashSendByte(Dummy_Byte);

  /* ���ô���Flash��CS�ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
  
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}
/*
funName	:spiFlashReadDeviceID
input		:
output	:uint32_t
describe:��ȡ����Flash�豸ID
remark	:
*/
uint32_t spiFlashReadDeviceID(void)
{
  uint32_t Temp = 0;

  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();

  /* ���������ȡоƬ�豸ID * */
  spiFlashSendByte(W25X_ManufactDeviceID);
  spiFlashSendByte(Dummy_Byte);
  spiFlashSendByte(Dummy_Byte);
  spiFlashSendByte(Dummy_Byte);
  
  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp = spiFlashSendByte(Dummy_Byte);

  /* ���ô���Flash��CS�ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();

  return Temp;
}
/*
funName	:spiFlashPowerDown
input		:
output	:
describe:�������ģʽ
remark	:
*/
void spiFlashPowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  spiFlashSendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE();
} 
/*
funName	:spiFlashReleasePowerDown
input		:
output	:
describe:�ͷŵ���ģʽ
remark	:
*/
void spiFlashReleasePowerDown(void)   
{
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  spiFlashSendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE(); 
}

void spiTest(void){
	uint8_t Tx_Buffer[] = "�����Ǹ������ӣ�";//�ַ�������и�\0,�������С���һ���ֽ�
	uint8_t Rx_Buffer[BufferSize];
	/* ����SPI��������д�� */
	spiFlashSectorErase(FLASH_SectorToErase);
	/* �����ͻ�����������д��flash�� */
	spiFlashBufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
	printf("write:%s\n", Tx_Buffer);
	/* ���ո�д������ݶ������ŵ����ջ������� */
	spiFlashBufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
	printf("read:%s\n", Rx_Buffer);
}
