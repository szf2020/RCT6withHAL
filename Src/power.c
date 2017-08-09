#include "power.h"
IWDG_HandleTypeDef hiwdg;
/*
funName	:enterStandbyMode
input		:void
output	:void
describe:进入待机模式
remark	:只有NRST,WK_UP,IWDG,RTC ALARM能重启
*/
void enterStandbyMode(void){
/* 禁用所有唤醒源: 唤醒引脚PA0 */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	/* 清除所有唤醒标志位 */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	
	/* 使能唤醒引脚：PA0做为系统唤醒输入,上升沿有效 */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	/* 进入待机模式 */
	HAL_PWR_EnterSTANDBYMode();
}
/*
funName	:Init_IWDG
input		:void
output	:ERROR_STUS
describe:初始化独立看门狗
remark	:T = 64 * 4000 / 40000 = 6.4s
*/
ERROR_STUS initIwdg(void){

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 4000;
	if(HAL_IWDG_Init(&hiwdg) != HAL_OK){
		return E_IWDG;
	}
	/* 启动独立看门狗 */
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
