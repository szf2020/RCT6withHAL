#ifndef _SIP_H_
#define _SIP_H_
#include "stm32f1xx_hal.h"
#include "Util.h"
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define TxBufferSize1   (countof(TxBuffer1))
#define RxBufferSize1   (countof(TxBuffer1))
#define BufferSize      (countof(Tx_Buffer))

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
#define W25X_WriteEnable		            0x06//дʹ�� 
#define W25X_WriteDisable		            0x04//д��ֹ
#define W25X_ReadStatusReg		          0x05//��״̬�Ĵ���1
#define W25X_WriteStatusReg		          0x01//д״̬�Ĵ���1
#define W25X_ReadData			              0x03//��ȡ����
#define W25X_FastReadData		            0x0B//���ٶ�ȡ����
#define W25X_FastReadDual		            0x3B//���ٶ�˫���
#define W25X_PageProgram		            0x02//ҳ���
#define W25X_BlockErase			            0xD8//64kb�Ŀ����
#define W25X_SectorErase		            0x20//��������
#define W25X_ChipErase			            0xC7//оƬ����
#define W25X_PowerDown			            0xB9//�������ģʽ
#define W25X_ReleasePowerDown	          0xAB//�ͷŵ���ģʽ
#define W25X_DeviceID			              0xAB//�豸ID,�������ָ��һ��
#define W25X_ManufactDeviceID   	      0x90//���������豸ID
#define W25X_JedecDeviceID		          0x9F//��jedec�豸ID
//Unique IDָ����4BH������û���õ�
#define WIP_Flag                        0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                      0xFF

#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET)

void initSpix(SPI_TypeDef *spix);
void spiFlashSectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void spiFlashBufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void spiFlashBufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t spiFlashReadID(void);
uint32_t spiFlashReadDeviceID(void);
void spiTest(void);

#endif
