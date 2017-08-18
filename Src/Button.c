#include "Button.h"
#include "Led.h"
#include "power.h"
#include "infrared.h"
BUTTON button = {0};

extern MACHINE_STATE machineState;
/*
funName	:initButton
input		:void
output	:void
describe:初始化按键
remark	:key0外部中断,key1扫描
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
  GPIO_InitStruct.Pull = GPIO_PULLUP;//外设下拉GND
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
/*
funName	:scanKey1
input		:void
output	:void
describe:key 1 按键响应
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
					printf("key1 press!enter stop mode\n");
					enterStopMode();
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
describe:长按key0
remark	:长按过程中会暂停程序其他操作，只能用于开机启动
*/
uint8_t longClickKey0(void)
{			
	uint8_t downCnt =0;																				//记录按下的次数
	uint8_t upCnt =0;																					//记录松开的次数			

	while(1)																										//死循环，由return结束
	{	
		HAL_Delay(20);																					//延迟一段时间再检测

		if(IsKey0Press)	//检测到按下按键
		{											
			downCnt++;																					//记录按下次数
			upCnt=0;																						//清除按键释放记录
			if(downCnt>=100)																		//按下时间足够
			{
        printf("长按key0!\n");	 
				return 1; 																				//检测到按键被时间长按下
			}
		}
		else 
		{
			upCnt++; 																						//记录释放次数
			if(upCnt>5)																					//连续检测到释放超过5次
			{
				return 0;																				//按下时间太短，不是按键长按操作
			}
		}
	}

}
/*
funName	:HAL_GPIO_EXTI_Callback
input		:GPIO_Pin
output	:void
describe:外部中断回调
remark	:key0外部中断,key1扫描
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	uint8_t pulse_time = 0;
	uint8_t leader_code_flag = 0; /* 引导码标志位，当引导码出现时，表示一帧数据开始 */
	uint8_t irda_data = 0;        /* 数据暂存位 */
	if(GPIO_Pin == GPIO_PIN_5){
		if(longClickKey0()){
			enterStandbyMode();
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
	}
	if(GPIO_Pin == GPIO_PIN_1){
		while(1)
		{
			if( INFRARED_DATA_IN== SET )        /* 只测量高电平的时间 */
			{       
				pulse_time = getPulseTime();
				
				/* >=5ms 不是有用信号 当出现干扰或者连发码时，也会break跳出while(1)循环 */
				if( pulse_time >= 250 )                
				{
					break; /* 跳出while(1)循环 */
				}
				
				if(pulse_time>=200 && pulse_time<250)         /* 获得前导位 4ms~4.5ms */
				{
					leader_code_flag = 1;
				}
				else if(pulse_time>=10 && pulse_time<50)      /* 0.56ms: 0.2ms~1ms */
				{
					irda_data = 0;
				}
				else if(pulse_time>=50 && pulse_time<100)     /* 1.68ms：1ms~2ms */
				{
					irda_data =1 ; 
				}        
				else if( pulse_time>=100 && pulse_time<=200 ) /* 2.1ms：2ms~4ms */
				{/* 连发码，在第二次中断出现 */
					frame_flag = 1;               /* 一帧数据接收完成 */
					frame_cnt++;                  /* 按键次数加1 */
					isr_cnt ++;                   /* 进中断一次加1 */
					LED1_TOGGLE();
					break;                        /* 跳出while(1)循环 */
				}
				
				if( leader_code_flag == 1 )
				{/* 在第一次中断中完成 */
					frame_data <<= 1;
					frame_data += irda_data;
					frame_cnt = 0;
					isr_cnt = 1;
				}
			}      
		} 
	}
}
