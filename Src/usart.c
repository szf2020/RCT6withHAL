#include "usart.h"
UART_HandleTypeDef huart1;//串口1
uint8_t aRxBuffer;//串口接收buffer
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
/*
funName	:enableRx
input		:void
output	:void
describe:使能串口接收
remark	:
*/
void uartEnableRx(void){
	/* 使能接收，进入中断回调函数 */
	HAL_UART_Receive_IT(&huart1,&aRxBuffer,1);
}
/*
funName	:HAL_UART_RxCpltCallback
input		:void
output	:void
describe:串口接收中断回调
remark	:
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//if(huart == &huart1){
		HAL_UART_Transmit(&huart1,&aRxBuffer,1,0);//反向回传
		HAL_UART_Receive_IT(&huart1,&aRxBuffer,1);//继续使能串口接收，不这样做串口接收只能进来一次
	//}
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
  }
}
