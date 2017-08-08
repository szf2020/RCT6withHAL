#include "power.h"
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
