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
	 /* 检测系统是否是从待机模式启动的 */ 
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
//    printf("待机唤醒复位\n");
    /* 清除待机标志位 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }
  else
  {
//    printf("系统是上电启动\n");
  }
}
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
funName	:sysclkConfig
input		:void
output	:void
describe:重新配置系统时钟
remark	:进入停止模式后，需要使能外部中断重新启动，启动后时钟为HSI，需要重新定义时钟
*/
void sysclkConfig(void)
{
  /* 使能 HSE */
  __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);

  /* 等待 HSE 准备就绪 */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET);
  
  /* 使能 PLL */ 
  __HAL_RCC_PLL_ENABLE();

  /* 等待 PLL 准备就绪 */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* 选择PLL作为系统时钟源 */
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

  /* 等待PLL被选择为系统时钟源 */
  while(__HAL_RCC_GET_SYSCLK_SOURCE() != 0x08)
  {
  }
}
/*
funName	:enterStopMode
input		:void
output	:void
describe:进入停止模式
remark	:进入停止模式后，需要使能外部中断重新启动，启动后时钟为HSI，需要重新定义时钟
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
	/* 进入睡眠模式，等待中断唤醒 */
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); 
	/* 退出睡眠模式之后，释放滴答定时器中断 */
	HAL_ResumeTick();
	startAllTimer();
	initUart(USART1);
	LED1_OFF();
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
/*
funName	:feedIwdg
input		:
output	:ERROR_STUS
describe:喂狗
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
describe:设置RTC日期
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
describe:设置RTC时间
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
describe:获取RTC日期和时间
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
describe:设置RTC闹钟
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
describe:停止RTC闹钟
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
describe:初始化RTC
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
describe:RTC闹钟中断回调
remark	:
*/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	machineState.RTCFlag = TRUE;
	LED1_TOGGLE();
}

