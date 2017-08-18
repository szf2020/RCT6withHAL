#include "Timer.h"
#include "Led.h"
BOOLEAN pwmRiseFlag = FALSE;//FALSE 变亮，ture 变暗
int16_t pwmValue = 0;//PWM值
PWM_SPEED pwmSpeed = SPEED3;//呼吸灯速度
uint16_t timercounter[8] = {0};//定时器计数数组
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;

extern MACHINE_STATE machineState;
/*
funName	:HAL_TIM_MspPostInit
input		:htim
output	:void
describe:定时器PWM输出通道引脚初始化
remark	:
*/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim->Instance==TIM1)
  {
    /**TIM1 GPIO Configuration    
    PA8     ------> TIM1_CH1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }

}

/*
funName	:BSP_initTimer
input		:	htim
					timer
					prescaler
					period
output	:void
describe:初始化任一定时器
remark	:
*/
void BSP_initTimer(TIM_HandleTypeDef *htim,TIM_TypeDef *timer,uint32_t prescaler,uint32_t period){
	TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim->Instance = timer;
  htim->Init.Prescaler = prescaler;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = period;
	
	if(timer != TIM6 || timer != TIM7){
		htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim->Init.RepetitionCounter = 0;
	}
	
  HAL_TIM_Base_Init(htim);
	
	if(timer != TIM6 || timer != TIM7){
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig);
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig);
	
	if(timer == TIM1){
		//TIM1_CH1:LED0,creat PWM,breathe led
		sConfigOC.OCMode = TIM_OCMODE_PWM2;
		sConfigOC.Pulse = pwmValue;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, TIM_CHANNEL_1);

		sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
		sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
		sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
		sBreakDeadTimeConfig.DeadTime = 0;
		sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
		sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
		sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
		HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig);

		HAL_TIM_MspPostInit(htim);
		HAL_TIM_PWM_Start(htim,TIM_CHANNEL_1);
	}
	
	HAL_TIM_Base_Start_IT(htim);
}
/*
funName	:ledPwm
input		:void
output	:void
describe:呼吸灯
remark	:
*/
void ledPwm(void){
	if(machineState.ledPwmFlag == TRUE){
		machineState.ledPwmFlag = FALSE;
		if(pwmRiseFlag == FALSE){
			pwmValue += pwmSpeed;
			if(pwmValue > PWM_MAX){
				pwmValue = PWM_MAX;
				pwmRiseFlag = TRUE;
			}
		}else{
			pwmValue -= pwmSpeed;
			if(pwmValue < 0){
				pwmValue = 0;
				pwmRiseFlag = FALSE;
			}
		}
		htim1.Instance->CCR1 = pwmValue;
	}
}
/*
funName	:HAL_TIM_PeriodElapsedCallback
input		:htim
output	:void
describe:定时器UP中断回调
remark	:
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM1){
		timercounter[0] ++;
		if(timercounter[0] % 10 == 0){
			machineState.buttonFlag = TRUE;
			machineState.ledPwmFlag = TRUE;
		}
	}else if(htim->Instance == TIM2){
		timercounter[1] ++;
		machineState.tim2Flag = TRUE;
		//定时器2
	}else if(htim->Instance == TIM3){
		timercounter[2] ++;
		machineState.tim3Flag = TRUE;
		//定时器3
	}else if(htim->Instance == TIM4){
		timercounter[3] ++;
		machineState.tim4Flag = TRUE;
		//定时器4
	}else if(htim->Instance == TIM5){
		timercounter[4] ++;
		machineState.tim5Flag = TRUE;
		//定时器5
	}else if(htim->Instance == TIM6){
		timercounter[5] ++;
		machineState.tim6Flag = TRUE;
		//定时器6
	}else if(htim->Instance == TIM7){
		timercounter[6] ++;
		machineState.tim7Flag = TRUE;
		//定时器7
	}else if(htim->Instance == TIM8){
		timercounter[7] ++;
		machineState.tim8Flag = TRUE;
		//定时器8
	}
}
/*
funName	:timerTest
input		:void
output	:void
describe:测试定时器2-8
remark	:
*/
void timerTest(void){
	if(machineState.tim2Flag == TRUE){
		machineState.tim2Flag = FALSE;
		printf("timercounter[1] = %d\n",timercounter[1]);
	}
	if(machineState.tim3Flag == TRUE){
		machineState.tim3Flag = FALSE;
		printf("timercounter[2] = %d\n",timercounter[2]);
	}
	if(machineState.tim4Flag == TRUE){
		machineState.tim4Flag = FALSE;
		//printf("TIM4");
	}
	if(machineState.tim5Flag == TRUE){
		machineState.tim5Flag = FALSE;
		//printf("TIM5");
	}
	if(machineState.tim6Flag == TRUE){
		machineState.tim6Flag = FALSE;
		//printf("TIM6");
	}
	if(machineState.tim7Flag == TRUE){
		machineState.tim7Flag = FALSE;
		//printf("TIM7");
	}
	if(machineState.tim8Flag == TRUE){
		machineState.tim8Flag = FALSE;
		//printf("TIM8");
	}
}
/*
funName	:initTimer
input		:timer
output	:ERROR_STUS
describe:
remark	:初始化定时器
*/
ERROR_STUS initTimer(TIM_TypeDef *timer){
	if(timer == TIM1){
		BSP_initTimer(&htim1,TIM1,71,1000);
	}else if(timer == TIM2){
		BSP_initTimer(&htim2,TIM2,71,20);
	}else if(timer == TIM3){
		BSP_initTimer(&htim3,TIM3,71,1000);
	}else if(timer == TIM4){
		BSP_initTimer(&htim4,TIM4,71,1000);
	}else if(timer == TIM5){
		BSP_initTimer(&htim5,TIM5,71,1000);
	}else if(timer == TIM6){
		BSP_initTimer(&htim6,TIM6,71,1000);
	}else if(timer == TIM7){
		BSP_initTimer(&htim7,TIM7,71,1000);
	}else if(timer == TIM8){
		BSP_initTimer(&htim8,TIM8,71,1000);
	}
	return E_OK;
}
/*
funName	:stopTimer
input		:timer
output	:
describe:停止某个定时器中断
remark	:
*/
void stopTimer(TIM_TypeDef *timer){
	if(timer == TIM1){
		HAL_TIM_Base_Stop_IT(&htim1);
	}else if(timer == TIM2){
		HAL_TIM_Base_Stop_IT(&htim2);
	}else if(timer == TIM3){
		HAL_TIM_Base_Stop_IT(&htim3);
	}else if(timer == TIM4){
		HAL_TIM_Base_Stop_IT(&htim4);
	}else if(timer == TIM5){
		HAL_TIM_Base_Stop_IT(&htim5);
	}else if(timer == TIM6){
		HAL_TIM_Base_Stop_IT(&htim6);
	}else if(timer == TIM7){
		HAL_TIM_Base_Stop_IT(&htim7);
	}else if(timer == TIM8){
		HAL_TIM_Base_Stop_IT(&htim8);
	}
}
/*
funName	:stopAllTimer
input		:
output	:
describe:停止所有定时器中断
remark	:
*/
void stopAllTimer(void){
	stopTimer(TIM1);
	stopTimer(TIM2);
	stopTimer(TIM3);
	stopTimer(TIM4);
	stopTimer(TIM5);
	stopTimer(TIM6);
	stopTimer(TIM7);
	stopTimer(TIM8);
}
/*
funName	:startTimer
input		:timer
output	:
describe:启动某个定时器
remark	:
*/
void startTimer(TIM_TypeDef *timer){
	if(timer == TIM1){
		HAL_TIM_Base_Start_IT(&htim1);
	}else if(timer == TIM2){
		HAL_TIM_Base_Start_IT(&htim2);
	}else if(timer == TIM3){
		HAL_TIM_Base_Start_IT(&htim3);
	}else if(timer == TIM4){
		HAL_TIM_Base_Start_IT(&htim4);
	}else if(timer == TIM5){
		HAL_TIM_Base_Start_IT(&htim5);
	}else if(timer == TIM6){
		HAL_TIM_Base_Start_IT(&htim6);
	}else if(timer == TIM7){
		HAL_TIM_Base_Start_IT(&htim7);
	}else if(timer == TIM8){
		HAL_TIM_Base_Start_IT(&htim8);
	}
}
/*
funName	:startAllTimer
input		:
output	:
describe:启动所有定时器
remark	:
*/
void startAllTimer(void){
	startTimer(TIM1);
	startTimer(TIM2);
	startTimer(TIM3);
	startTimer(TIM4);
	startTimer(TIM5);
	startTimer(TIM6);
	startTimer(TIM7);
	startTimer(TIM8);
}

