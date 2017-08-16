#include "Spi.h"
SPI_HandleTypeDef hspiflash;



/*
funName	:initSpi
input		:*spix
output	:
describe:初始化spix
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
describe:复写初始化spigpio
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
describe:spi有返回的发送一个字节，如果发送失败返回0xff
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
describe:spi 写使能
remark	:
*/
void spiFlashWriteEnable(void)
{
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送命令：写使能 */
  spiFlashSendByte(W25X_WriteEnable);

  /* 禁用串行Flash：CS高电平 */
  FLASH_SPI_CS_DISABLE();
}
/*
funName	:spiFlashWaitForWriteEnd
input		:
output	:
describe:spi 等待数据写入完成
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
describe:spi 擦除扇区
remark	:
*/
void spiFlashSectorErase(uint32_t SectorAddr)
{
  /* 发送FLASH写使能命令 */
  spiFlashWriteEnable();
  spiFlashWaitForWriteEnd();
  /* 擦除扇区 */
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();
  /* 发送扇区擦除指令*/
  spiFlashSendByte(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  spiFlashSendByte((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  spiFlashSendByte((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  spiFlashSendByte(SectorAddr & 0xFF);
  /* 禁用串行FLASH: CS 高电平 */
  FLASH_SPI_CS_DISABLE();
  /* 等待擦除完毕*/
  spiFlashWaitForWriteEnd();
}
/*
funName	:spiFlashBulkErase
input		:
output	:
describe:spi 擦除芯片
remark	:
*/
void spiFlashBulkErase(void)
{
 /* 发送FLASH写使能命令 */
  spiFlashWriteEnable();

  /* 整片擦除 Erase */
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();
  /* 发送整片擦除指令*/
  spiFlashSendByte(W25X_ChipErase);
  /* 禁用串行FLASH: CS高电平 */
  FLASH_SPI_CS_DISABLE();

  /* 等待擦除完毕*/
  spiFlashWaitForWriteEnd();
}
/*
funName	:spiFlashPageWrite
input		:
pBuffer：待写入数据的指针
WriteAddr：写入地址
NumByteToWrite：写入数据长度
output	:
describe:spi 页写入，往串行FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
remark	: 串行Flash每页大小为256个字节
*/
void spiFlashPageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* 发送FLASH写使能命令 */
  spiFlashWriteEnable();

   /* 寻找串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();
  /* 写送写指令*/
  spiFlashSendByte(W25X_PageProgram);
  /*发送写地址的高位*/
  spiFlashSendByte((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  spiFlashSendByte((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  spiFlashSendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("Err: SPI_FLASH_PageWrite too large!\n");
  }

  /* 写入数据*/
  while (NumByteToWrite--)
  {
     /* 发送当前要写入的字节数据 */
    spiFlashSendByte(*pBuffer);
     /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 禁用串行FLASH: CS 高电平 */
  FLASH_SPI_CS_DISABLE();

  /* 等待写入完毕*/
  spiFlashWaitForWriteEnd();
}
/*
funName	:spiFlashBufferWrite
input		:
pBuffer：待写入数据的指针
WriteAddr：写入地址
NumByteToWrite：写入数据长度
output	:
describe:往串行FLASH写入数据，调用本函数写入数据前需要先擦除扇区
remark	: 该函数可以设置任意写入数据长度
*/
void spiFlashBufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* 若地址与 SPI_FLASH_PageSize 对齐  */
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
  else /* 若地址与 SPI_FLASH_PageSize 不对齐 */
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
pBuffer：存放读取到数据的指针
ReadAddr：读取数据目标地址
NumByteToRead：读取数据长度
output	:
describe:读取flash数据
remark	: 该函数可以设置任意读取数据长度
*/
void spiFlashBufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送 读 指令 */
  spiFlashSendByte(W25X_ReadData);

  /* 发送 读 地址高位 */
  spiFlashSendByte((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  spiFlashSendByte((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  spiFlashSendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* 读取数据 */
  {
     /* 读取一个字节*/
    *pBuffer = spiFlashSendByte(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 禁用串行FLASH: CS 高电平 */
  FLASH_SPI_CS_DISABLE();
}
/*
funName	:spiFlashReadID
input		:
output	:
describe:读取flash型号ID
remark	:24个字节
FLASH_ID      IC型号      存储空间大小         
							0xEF3015      W25X16        2M byte
							0xEF4015	    W25Q16        4M byte
							0XEF4017      W25Q64        8M byte
							0XEF4018      W25Q128       16M byte
*/
uint32_t spiFlashReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送命令：读取芯片型号ID */
  spiFlashSendByte(W25X_JedecDeviceID);

  /* 从串行Flash读取一个字节数据 */
  Temp0 = spiFlashSendByte(Dummy_Byte);

  /* 从串行Flash读取一个字节数据 */
  Temp1 = spiFlashSendByte(Dummy_Byte);

  /* 从串行Flash读取一个字节数据 */
  Temp2 = spiFlashSendByte(Dummy_Byte);

  /* 禁用串行Flash：CS高电平 */
  FLASH_SPI_CS_DISABLE();
  
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}
/*
funName	:spiFlashReadDeviceID
input		:
output	:uint32_t
describe:读取串行Flash设备ID
remark	:
*/
uint32_t spiFlashReadDeviceID(void)
{
  uint32_t Temp = 0;

  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送命令：读取芯片设备ID * */
  spiFlashSendByte(W25X_ManufactDeviceID);
  spiFlashSendByte(Dummy_Byte);
  spiFlashSendByte(Dummy_Byte);
  spiFlashSendByte(Dummy_Byte);
  
  /* 从串行Flash读取一个字节数据 */
  Temp = spiFlashSendByte(Dummy_Byte);

  /* 禁用串行Flash：CS高电平 */
  FLASH_SPI_CS_DISABLE();

  return Temp;
}
/*
funName	:spiFlashPowerDown
input		:
output	:
describe:进入掉电模式
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
describe:释放掉电模式
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
	uint8_t Tx_Buffer[] = {1,2,3,45,6};//字符串最后有个\0,所以其大小会多一个字节
	uint8_t Rx_Buffer[BufferSize] = {0};
	uint8_t i;
	/* 擦除SPI的扇区以写入 */
	spiFlashSectorErase(FLASH_SectorToErase);
	/* 将发送缓冲区的数据写到flash中 */
	spiFlashBufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
	for(i = 0; i < BufferSize; i ++){
		printf("write:%d\n", Tx_Buffer[i]);
	}
	//printf("write:%d\n", Tx_Buffer);
	/* 将刚刚写入的数据读出来放到接收缓冲区中 */
	spiFlashBufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
	for(i = 0; i < BufferSize; i ++){
		printf("read:%d\n", Rx_Buffer[i]);
	}
	//printf("read:%s\n", Rx_Buffer);
}
