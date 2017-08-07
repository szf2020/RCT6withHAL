#include "power.h"
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
