#include "Infrared.h"
#include "Timer.h"
/* 私有变量 ------------------------------------------------------------------*/
uint32_t frame_data=0;    /* 一帧数据缓存 */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* 一帧数据接收完成标志 */
uint8_t isr_cnt;
/*
funName	:initInfrared
input		:
output	:
describe:初始化PA1 红外接收HS0038 下降沿中断
remark	:
*/
void initInfrared(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	/*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
/*
funName	:getPulseTime
input		:
output	:
describe:获取高电平时间
remark	:timercounter[1] 20us定时器
*/
uint8_t getPulseTime(void)
{
	timercounter[1] = 0;
  while(INFRARED_DATA_IN)
  {
    if(timercounter[1] >= 250)
      return timercounter[1];   // 超时溢出   
  }
  return timercounter[1];
}
/*
funName	:getProcessData
input		:
output	:uint8_t 键值
describe:获取键值
remark	:
*/
uint8_t getProcessData(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* 记得清标志位 */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (uint8_t)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* 错误返回 */
}
/*
funName	:infraredTest
input		:
output	:
describe:测试红外接收
remark	:
*/
void infraredTest(void){
	if( frame_flag == 1 ) /* 一帧红外数据接收完成 */
	{
		printf("key_val=%d \n",getProcessData());
		printf("按键次数frame_cnt=%d \n",frame_cnt);
		printf("中断次数isr_cnt=%d \n",isr_cnt);
	}
}
