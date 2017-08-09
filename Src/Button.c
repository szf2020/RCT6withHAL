#include "Button.h"
#include "Led.h"
#include "power.h"
BUTTON button = {0};

extern MACHINE_STATE machineState;
/*
funName	:initButton
input		:void
output	:void
describe:��ʼ������
remark	:key0�ⲿ�ж�,key1ɨ��
*/
void initButton(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	/*Configure GPIO pin : KEY0_EXIT_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;//��������GND
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
/*
funName	:scanKey1
input		:void
output	:void
describe:key 1 ������Ӧ
remark	:
*/
void scanKey1(void){
	if(machineState.buttonFlag == TRUE){
		machineState.buttonFlag = FALSE;
		if(button.keyflag[1] == 0){
			if(IsKey1Press){
				button.keycounter[1] ++;
				if(button.keycounter[1] >= 10){
					button.keycounter[1] = 0;
					button.keyflag[1] = 1;
					printf("key1 press!\n");
				}
			}else{
				button.keycounter[1] = 0;
			}
			
		}else if(button.keyflag[1] == 1){
			if(IsKey1Press){
				button.keycounter[1] = 0;
			}else{
				button.keycounter[1] ++;
				if(button.keycounter[1] >= 10){
					button.keycounter[1] = 0;
					button.keyflag[1] = 0;
				}
			}
		}
	}
	
}
/*
funName	:longClickKey0
input		:void
output	:uint8_t
describe:����key0
remark	:���������л���ͣ��������������ֻ�����ڿ�������
*/
uint8_t longClickKey0(void)
{			
	uint8_t downCnt =0;																				//��¼���µĴ���
	uint8_t upCnt =0;																					//��¼�ɿ��Ĵ���			

	while(1)																										//��ѭ������return����
	{	
		HAL_Delay(20);																					//�ӳ�һ��ʱ���ټ��

		if(IsKey0Press)	//��⵽���°���
		{											
			downCnt++;																					//��¼���´���
			upCnt=0;																						//��������ͷż�¼
			if(downCnt>=100)																		//����ʱ���㹻
			{
        printf("����key0!\n");	 
				return 1; 																				//��⵽������ʱ�䳤����
			}
		}
		else 
		{
			upCnt++; 																						//��¼�ͷŴ���
			if(upCnt>5)																					//������⵽�ͷų���5��
			{
				return 0;																				//����ʱ��̫�̣����ǰ�����������
			}
		}
	}

}
/*
funName	:HAL_GPIO_EXTI_Callback
input		:GPIO_Pin
output	:void
describe:�ⲿ�жϻص�
remark	:key0�ⲿ�ж�,key1ɨ��
*/
uint8_t i,j;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_5){
		if(longClickKey0()){
			enterStandbyMode();
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
	}
}
