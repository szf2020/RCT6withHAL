#include "Infrared.h"
#include "Timer.h"
/* ˽�б��� ------------------------------------------------------------------*/
uint32_t frame_data=0;    /* һ֡���ݻ��� */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* һ֡���ݽ�����ɱ�־ */
uint8_t isr_cnt;
/*
funName	:initInfrared
input		:
output	:
describe:��ʼ��PA1 �������HS0038 �½����ж�
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
describe:��ȡ�ߵ�ƽʱ��
remark	:timercounter[1] 20us��ʱ��
*/
uint8_t getPulseTime(void)
{
	timercounter[1] = 0;
  while(INFRARED_DATA_IN)
  {
    if(timercounter[1] >= 250)
      return timercounter[1];   // ��ʱ���   
  }
  return timercounter[1];
}
/*
funName	:getProcessData
input		:
output	:uint8_t ��ֵ
describe:��ȡ��ֵ
remark	:
*/
uint8_t getProcessData(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* �ǵ����־λ */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (uint8_t)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* ���󷵻� */
}
/*
funName	:infraredTest
input		:
output	:
describe:���Ժ������
remark	:
*/
void infraredTest(void){
	if( frame_flag == 1 ) /* һ֡�������ݽ������ */
	{
		printf("key_val=%d \n",getProcessData());
		printf("��������frame_cnt=%d \n",frame_cnt);
		printf("�жϴ���isr_cnt=%d \n",isr_cnt);
	}
}
