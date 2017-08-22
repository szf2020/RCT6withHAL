#include "usart.h"
#include "Esp8266.h"
UART_HandleTypeDef huart1;//串口1
UART_HandleTypeDef huart2;//串口2
uint8_t aRxBuffer[2];//串口接收buffer,aRxBuffer[0]接收串口1，aRxBuffer[1]接收串口2
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){}//循环发送,直到发送完毕   
  USART1->DR = (uint8_t) ch;      
	return ch;
}
#endif 
#if 0
/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
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
describe:使能串口接收
remark	:
*/
void uartEnableRx(UART_HandleTypeDef *huart){
	if(huart == &huart1){
		/* 使能接收，进入中断回调函数 */
		HAL_UART_Receive_IT(&huart1,&aRxBuffer[0],1);
	}else if(huart == &huart2){
		HAL_UART_Receive_IT(&huart2,&aRxBuffer[1],1);
	}
}
/*
funName	:HAL_UART_RxCpltCallback
input		:*huart
output	:void
describe:串口接收中断回调
remark	:
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1){
		HAL_UART_Transmit(&huart1,&aRxBuffer[0],1,0);//反向回传
		HAL_UART_Receive_IT(&huart1,&aRxBuffer[0],1);//继续使能串口接收，不这样做串口接收只能进来一次
	}else if(huart == &huart2){
		if(espRxFram.InfBit.FramLength < RX_BUF_MAX_LEN - 1){
			espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength ++] = aRxBuffer[1];
		}
		HAL_UART_Receive_IT(&huart2,&aRxBuffer[1],1);//继续使能串口接收，不这样做串口接收只能进来一次
	}
}
/*
funName	:HAL_UART_MspInit
input		:uartHandle 串口句柄
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
describe:串口初始化
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
describe:初始化其中一个串口
remark	:
*/
void initUart(USART_TypeDef *uart){
	if(uart == USART1){
		uartInit(&huart1,USART1,115200);
	}else if(uart == USART2){
		uartInit(&huart2,USART2,115200);
	}
}
