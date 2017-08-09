#include "Timer.h"
#include "Led.h"
BOOLEAN pwmRiseFlag = FALSE;//FALSE ������ture �䰵
uint16_t pwmValue = 0;
PWM_SPEED pwmSpeed = SPEED4;
uint16_t timercounter[8] = {0};
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
describe:��ʱ��PWM���ͨ�����ų�ʼ��
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
describe:��ʼ����һ��ʱ��
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
describe:������
remark	:
*/
void ledPwm(void){
	if(machineState.ledPwmFlag == TRUE){
		machineState.ledPwmFlag = FALSE;
		if(pwmRiseFlag == FALSE){
			if(pwmValue < PWM_MAX){
				pwmValue += pwmSpeed;
			}else{
				pwmRiseFlag = TRUE;
			}
		}else{
			if(pwmValue > 0){
				pwmValue -= pwmSpeed;
			}else{
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
describe:��ʱ��UP�жϻص�
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
		//��ʱ��2
	}else if(htim->Instance == TIM3){
		//��ʱ��3
	}else if(htim->Instance == TIM4){
		//��ʱ��4
	}else if(htim->Instance == TIM5){
		//��ʱ��5
	}else if(htim->Instance == TIM6){
		//��ʱ��6
	}else if(htim->Instance == TIM7){
		//��ʱ��7
	}else if(htim->Instance == TIM8){
		//��ʱ��8
	}
}
/*
funName	:
input		:
output	:
describe:
remark	:
*/
void initTimer(TIM_TypeDef *timer){
	if(timer == TIM1){
		BSP_initTimer(&htim1,TIM1,71,1000);
	}else if(timer == TIM2){
		BSP_initTimer(&htim2,TIM2,71,1000);
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
}
