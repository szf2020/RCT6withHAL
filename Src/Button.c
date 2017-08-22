#include "Button.h"
#include "Led.h"
#include "power.h"
#include "infrared.h"
#include "Esp8266.h"
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
  GPIO_InitStruct.Pull = GPIO_PULLUP;
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
//					printf("key1 press!enter stop mode\n");
//					enterStopMode();
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
//        printf("����key0!\n");	 
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	uint8_t pulse_time = 0;
	uint8_t leader_code_flag = 0; /* �������־λ�������������ʱ����ʾһ֡���ݿ�ʼ */
	uint8_t irda_data = 0;        /* �����ݴ�λ */
	if(GPIO_Pin == GPIO_PIN_5){
		if(longClickKey0()){
			enterStandbyMode();
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
	}
	if(GPIO_Pin == GPIO_PIN_1){
		while(1)
		{
			if( INFRARED_DATA_IN== SET )        /* ֻ�����ߵ�ƽ��ʱ�� */
			{       
				pulse_time = getPulseTime();
				
				/* >=5ms ���������ź� �����ָ��Ż���������ʱ��Ҳ��break����while(1)ѭ�� */
				if( pulse_time >= 250 )                
				{
					break; /* ����while(1)ѭ�� */
				}
				
				if(pulse_time>=200 && pulse_time<250)         /* ���ǰ��λ 4ms~4.5ms */
				{
					leader_code_flag = 1;
				}
				else if(pulse_time>=10 && pulse_time<50)      /* 0.56ms: 0.2ms~1ms */
				{
					irda_data = 0;
				}
				else if(pulse_time>=50 && pulse_time<100)     /* 1.68ms��1ms~2ms */
				{
					irda_data =1 ; 
				}        
				else if( pulse_time>=100 && pulse_time<=200 ) /* 2.1ms��2ms~4ms */
				{/* �����룬�ڵڶ����жϳ��� */
					frame_flag = 1;               /* һ֡���ݽ������ */
					frame_cnt++;                  /* ����������1 */
					isr_cnt ++;                   /* ���ж�һ�μ�1 */
					LED1_TOGGLE();
					break;                        /* ����while(1)ѭ�� */
				}
				
				if( leader_code_flag == 1 )
				{/* �ڵ�һ���ж������ */
					frame_data <<= 1;
					frame_data += irda_data;
					frame_cnt = 0;
					isr_cnt = 1;
				}
			}      
		} 
	}
}
