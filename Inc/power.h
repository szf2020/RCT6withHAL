#ifndef _POWER_H_
#define _POWER_H_
#include "stm32f1xx_hal.h"

typedef struct{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t alarmHour;
	uint8_t alarmMinute;
	uint8_t alarmSecond;
}RTC_TIME;

void enterStandbyMode(void);
void enterStopMode(void);
void enterSleepMode(void);
ERROR_STUS initIwdg(void);
ERROR_STUS feedIwdg(void);
void checkPowerOnMode(void);
void checkPowerMode(void);

ERROR_STUS initRTC(void);
RTC_TIME getRTCDateAndTime(void);
ERROR_STUS RTCAlarmDisable(void);
#endif
