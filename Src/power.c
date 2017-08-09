#include "power.h"
#include "Timer.h"
#include "SystemConfig.h"
#include "Led.h"
#include "usart.h"
IWDG_HandleTypeDef hiwdg;

void checkPowerMode(void){
	 /* ���ϵͳ�Ƿ��ǴӴ���ģʽ������ */ 
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    printf("�������Ѹ�λ\n");
    /* ���������־λ */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }
  else
  {
    printf("ϵͳ���ϵ�����\n");
  }
}
/*
funName	:enterStandbyMode
input		:void
output	:void
describe:�������ģʽ
remark	:ֻ��NRST,WK_UP,IWDG,RTC ALARM������
*/
void enterStandbyMode(void){
/* �������л���Դ: ��������PA0 */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	/* ������л��ѱ�־λ */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	
	/* ʹ�ܻ������ţ�PA0��Ϊϵͳ��������,��������Ч */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	/* �������ģʽ */
	HAL_PWR_EnterSTANDBYMode();
}
/*
funName	:sysclkConfig
input		:void
output	:void
describe:��������ϵͳʱ��
remark	:����ֹͣģʽ����Ҫʹ���ⲿ�ж�����������������ʱ��ΪHSI����Ҫ���¶���ʱ��
*/
void sysclkConfig(void)
{
  /* ʹ�� HSE */
  __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);

  /* �ȴ� HSE ׼������ */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET);
  
  /* ʹ�� PLL */ 
  __HAL_RCC_PLL_ENABLE();

  /* �ȴ� PLL ׼������ */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* ѡ��PLL��Ϊϵͳʱ��Դ */
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

  /* �ȴ�PLL��ѡ��Ϊϵͳʱ��Դ */
  while(__HAL_RCC_GET_SYSCLK_SOURCE() != 0x08)
  {
  }
}
/*
funName	:enterStopMode
input		:void
output	:void
describe:����ֹͣģʽ
remark	:����ֹͣģʽ����Ҫʹ���ⲿ�ж�����������������ʱ��ΪHSI����Ҫ���¶���ʱ��
*/
void enterStopMode(void){
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);
	sysclkConfig();
}
void enterSleepMode(void){
	LED1_ON();
	deInitUart(USART1);
	stopAllTimer();
	HAL_SuspendTick();
	/* ����˯��ģʽ���ȴ��жϻ��� */
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); 
	/* �˳�˯��ģʽ֮���ͷŵδ�ʱ���ж� */
	HAL_ResumeTick();
	startAllTimer();
	initUart(USART1);
	LED1_OFF();
}
/*
funName	:Init_IWDG
input		:void
output	:ERROR_STUS
describe:��ʼ���������Ź�
remark	:T = 64 * 4000 / 40000 = 6.4s
*/
ERROR_STUS initIwdg(void){

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 4000;
	if(HAL_IWDG_Init(&hiwdg) != HAL_OK){
		return E_IWDG;
	}
	/* �����������Ź� */
	if(HAL_IWDG_Start(&hiwdg)!= HAL_OK){
		return E_IWDG;
	}
	return E_OK;
}
/*
funName	:feedIwdg
input		:
output	:ERROR_STUS
describe:ι��
remark	:
*/
ERROR_STUS feedIwdg(void){
	if(HAL_IWDG_Refresh(&hiwdg) != HAL_OK){
		return E_FEEDIWDG;
	}
	return E_OK;
}
