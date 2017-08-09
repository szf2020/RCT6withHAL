#include "Button.h"
#include "Led.h"
#include "power.h"
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
  GPIO_InitStruct.Pull = GPIO_NOPULL;
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
uint8_t i,j;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_5){
		if(longClickKey0()){
			enterStandbyMode();
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
	}
}
