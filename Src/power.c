#include "power.h"
#include "Timer.h"
#include "SystemConfig.h"
#include "Led.h"
#include "usart.h"
IWDG_HandleTypeDef hiwdg;
RTC_HandleTypeDef hrtc;
RTC_DateTypeDef sDate;
RTC_TimeTypeDef sTime;
RTC_AlarmTypeDef RTC_alarm;

extern MACHINE_STATE machineState;

void checkPowerMode(void){
	 /* ���ϵͳ�Ƿ��ǴӴ���ģʽ������ */ 
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
//    printf("�������Ѹ�λ\n");
    /* ���������־λ */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }
  else
  {
//    printf("ϵͳ���ϵ�����\n");
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
/*
funName	:setRTCDate
input		:
output	:ERROR_STUS
describe:����RTC����
remark	:
*/
ERROR_STUS setRTCDate(uint8_t year,uint8_t month,uint8_t day,uint8_t week){
	sDate.Year = year;
	sDate.Month = month;
	sDate.Date = day;
	sDate.WeekDay = week;
	if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK){
		return E_RTC;
	}
	return E_OK;
}
/*
funName	:setRTCTime
input		:
output	:ERROR_STUS
describe:����RTCʱ��
remark	:
*/
ERROR_STUS setRTCTime(uint8_t hour,uint8_t minute,uint8_t second){
	sTime.Hours = hour;
	sTime.Minutes = minute;
	sTime.Seconds = second;
	if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK){
		return E_RTC;
	}
	return E_OK;
}
/*
funName	:getRTCDateAndTime
input		:
output	:RTC_TIME
describe:��ȡRTC���ں�ʱ��
remark	:
*/
RTC_TIME getRTCDateAndTime(void){
	RTC_TIME rtcTime;
	
	HAL_RTC_GetTime(&hrtc, &sTime,RTC_FORMAT_BIN);
	
	HAL_RTC_GetDate(&hrtc, &sDate,RTC_FORMAT_BIN);
	
	HAL_RTC_GetAlarm(&hrtc,&RTC_alarm,RTC_ALARM_A,RTC_FORMAT_BIN);
	
	rtcTime.year 	= sDate.Year;
	rtcTime.month = sDate.Month;
	rtcTime.day 	= sDate.Date;
	rtcTime.week 	= sDate.WeekDay;
	
	rtcTime.hour = sTime.Hours;
	rtcTime.minute = sTime.Minutes;
	rtcTime.second = sTime.Seconds;
	
	rtcTime.alarmHour = RTC_alarm.AlarmTime.Hours;
	rtcTime.alarmMinute = RTC_alarm.AlarmTime.Minutes;
	rtcTime.alarmSecond = RTC_alarm.AlarmTime.Seconds;
	
	return rtcTime;
}
/*
funName	:setRTCAlarm
input		:
output	:ERROR_STUS
describe:����RTC����
remark	:
*/
ERROR_STUS setRTCAlarm(uint8_t hour,uint8_t minute,uint8_t second){
	
	
	getRTCDateAndTime();
	
	RTC_alarm.Alarm = RTC_ALARM_A;
	RTC_alarm.AlarmTime.Hours = sTime.Hours + hour;
	RTC_alarm.AlarmTime.Minutes = sTime.Minutes + minute;
	RTC_alarm.AlarmTime.Seconds = sTime.Seconds + second;
	
	if(HAL_RTC_SetAlarm_IT(&hrtc, &RTC_alarm, RTC_FORMAT_BIN) != HAL_OK){
		return E_RTCALARM;
	}
	return E_OK;
}
/*
funName	:RTCAlarmDisable
input		:
output	:ERROR_STUS
describe:ֹͣRTC����
remark	:
*/
ERROR_STUS RTCAlarmDisable(void){
	if(HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A) != HAL_OK){
		return E_RTCALARM;
	}
	return E_OK;
}
/*
funName	:initRTC
input		:
output	:ERROR_STUS
describe:��ʼ��RTC
remark	:
*/
ERROR_STUS initRTC(void){
	
	hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	if (HAL_RTC_Init(&hrtc) != HAL_OK){
		return E_RTC;
	}
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x5448){
		if(setRTCDate(17,RTC_MONTH_AUGUST,11,RTC_WEEKDAY_FRIDAY) != E_OK){
			return E_RTC;
		}
		if(setRTCTime(0,0,0) != E_OK){
			return E_RTC;
		}
	}else{
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
	if(setRTCAlarm(0,0,3) != E_OK){
		return E_RTCALARM;
	}
	return E_OK;
}
/*
funName	:HAL_RTC_AlarmAEventCallback
input		:
output	:void
describe:RTC�����жϻص�
remark	:
*/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	machineState.RTCFlag = TRUE;
	LED1_TOGGLE();
}

