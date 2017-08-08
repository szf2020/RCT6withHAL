#include "usart.h"
UART_HandleTypeDef huart1;//����1
uint8_t aRxBuffer;//���ڽ���buffer
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
		HAL_UART_Receive_IT(&huart1,&aRxBuffer,1);
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
		HAL_UART_Transmit(&huart1,&aRxBuffer,1,0);//����ش�
		HAL_UART_Receive_IT(&huart1,&aRxBuffer,1);//����ʹ�ܴ��ڽ��գ������������ڽ���ֻ�ܽ���һ��
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
  }
}
/*
funName	:MX_USART1_UART_Init
input		:*huart
				 *uart
					baud
output	:HAL_StatusTypeDef
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
void initUart(USART_TypeDef *uart){
	if(uart == USART1){
		uartInit(&huart1,USART1,115200);
	}
}
