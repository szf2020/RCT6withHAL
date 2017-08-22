#include "usart.h"
#include "Esp8266.h"
UART_HandleTypeDef huart1;//����1
UART_HandleTypeDef huart2;//����2
uint8_t aRxBuffer[2];//���ڽ���buffer,aRxBuffer[0]���մ���1��aRxBuffer[1]���մ���2
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){}//ѭ������,ֱ���������   
  USART1->DR = (uint8_t) ch;      
	return ch;
}
#endif 
#if 0
/**
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1,&ch, 1, 0xffff);
  return ch;
}
#endif
/*
funName	:enableRx
input		:*huart
output	:void
describe:ʹ�ܴ��ڽ���
remark	:
*/
void uartEnableRx(UART_HandleTypeDef *huart){
	if(huart == &huart1){
		/* ʹ�ܽ��գ������жϻص����� */
		HAL_UART_Receive_IT(&huart1,&aRxBuffer[0],1);
	}else if(huart == &huart2){
		HAL_UART_Receive_IT(&huart2,&aRxBuffer[1],1);
	}
}
/*
funName	:HAL_UART_RxCpltCallback
input		:*huart
output	:void
describe:���ڽ����жϻص�
remark	:
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1){
		HAL_UART_Transmit(&huart1,&aRxBuffer[0],1,0);//����ش�
		HAL_UART_Receive_IT(&huart1,&aRxBuffer[0],1);//����ʹ�ܴ��ڽ��գ������������ڽ���ֻ�ܽ���һ��
	}else if(huart == &huart2){
		if(espRxFram.InfBit.FramLength < RX_BUF_MAX_LEN - 1){
			espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength ++] = aRxBuffer[1];
		}
		HAL_UART_Receive_IT(&huart2,&aRxBuffer[1],1);//����ʹ�ܴ��ڽ��գ������������ڽ���ֻ�ܽ���һ��
	}
}
/*
funName	:HAL_UART_MspInit
input		:uartHandle ���ھ��
output	:NA
describe:init usart1 IO and NVIC
remark	:
*/
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART1)
  {
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }else if(uartHandle->Instance==USART2)
  {
    /**USART1 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}
/*
funName	:MX_USART1_UART_Init
input		:*huart
				 *uart
					baud
output	:ERROR_STUS
describe:���ڳ�ʼ��
remark	:
*/
ERROR_STUS uartInit(UART_HandleTypeDef *huart,USART_TypeDef *uart,uint32_t baud)
{
  huart->Instance = uart;
  huart->Init.BaudRate = baud;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if(HAL_UART_Init(huart) != HAL_OK){
		return E_UART;
	}
	uartEnableRx(huart);
	return E_OK;
}
void deInitUart(USART_TypeDef *uart){
	if(uart == USART1){
		HAL_UART_DeInit(&huart1);
	}
}
/*
funName	:initUart
input		:*uart
output	:void
describe:��ʼ������һ������
remark	:
*/
void initUart(USART_TypeDef *uart){
	if(uart == USART1){
		uartInit(&huart1,USART1,115200);
	}else if(uart == USART2){
		uartInit(&huart2,USART2,115200);
	}
}
