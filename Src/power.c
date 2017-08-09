#include "power.h"
#include "Timer.h"
#include "SystemConfig.h"
IWDG_HandleTypeDef hiwdg;
/*
funName	:enterStandbyMode
input		:void
output	:void
describe:�������ģʽ
remark	:ֻ��NRST,WK_UP,IWDG,RTC ALARM������
*/
void enterStandbyMode(void){
	rccClockDisable();
/* �������л���Դ: ��������PA0 */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	/* ������л��ѱ�־λ */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	
	/* ʹ�ܻ������ţ�PA0��Ϊϵͳ��������,��������Ч */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	/* �������ģʽ */
	HAL_PWR_EnterSTANDBYMode();
}
void checkPowerOnMode(void){
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
ERROR_STUS feedIwdg(void){
	if(HAL_IWDG_Refresh(&hiwdg) != HAL_OK){
		return E_FEEDIWDG;
	}
	return E_OK;
}
