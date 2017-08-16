/*
funName	:
input		:
output	:
describe:
remark	:
*/
#include "usart.h"
#include "power.h"
#include "Led.h"
#include "SystemConfig.h"
#include "Button.h"
#include "Timer.h"
#include "lcd.h"
#include "AdcDma.h"
#include "Iic.h"
#include "Spi.h"
#include "Touch.h"
MACHINE_STATE machineState;
ERROR_STUS errorStus = E_OK;
RTC_TIME rtcTime;
/* ���ڱ���ת�������ĵ�ѹֵ */	 
float Current_Temperature[2];
uint8_t test[100] = {0};
uint32_t FlashID = 0;
uint32_t DeviceID = 0;
uint32_t i;
int main(void)
{
	errorStus = setPriorityGroup();
	errorStus = systemClockConfig();
	
	rccClockEnable();
	
	initUart(USART1);
	initLed();
	initButton();
	initTimer(TIM1);
	initTimer(TIM2);
	initTimer(TIM3);
	initTimer(TIM4);
	initTimer(TIM5);
	initTimer(TIM6);
	initTimer(TIM7);
	initTimer(TIM8);
	initTFTLCD();
	initRTC();
	initADC1();
	errorStus = initIic();
//	if(errorStus == E_OK){
//		iicTest();
//	}
	initSpix(SPI1);
	DeviceID = spiFlashReadDeviceID();
	HAL_Delay(100);
	FlashID = spiFlashReadID();
	printf("FlashID is 0x%X,  Manufacturer Device ID is 0x%X\n", FlashID, DeviceID);
	spiTest();
	touchDev.init();

  /* last set */
	//initIwdg();
  setPriority();
	/* pre init set*/
	checkPowerMode();
	//enterSleepMode();
//	POINT_COLOR = RED;
//	LCD_Set_Window(100,100,100,60);
//	POINT_COLOR = BLUE;
  while (1)
  {
		scanKey1();
		ledPwm();
		touchDev.scan(0);
//		rtcTime = getRTCDateAndTime();
//		Current_Temperature[0] = checkChipTemp();
//		Current_Temperature[1] = ADCPA1transform();
//		printf("%d->%3f\n",ADC_ConvertedValue[0],Current_Temperature[0]);
//		printf("%d->%3f\n",ADC_ConvertedValue[1],Current_Temperature[1]);
//		sprintf(test,"%d:%d:%d:%d--%d:%d:%d--%d:%d:%d\n",rtcTime.year,rtcTime.month,rtcTime.day,rtcTime.week,rtcTime.hour,rtcTime.minute,rtcTime.second,rtcTime.alarmHour,rtcTime.alarmMinute,rtcTime.alarmSecond);
//		printf("%s",test);
		
		//LCD_Clear(GREEN);
		//printf("1111");
//		LCD_DrawPoint(10,10);
//		printf("%x\n",LCD_ReadPoint(10,10));
//		LCD_DrawLine(20,20,100,100);
//		LCD_DrawRectangle(40,40,60,60);
//		LCD_Draw_Circle(100,100,50);
//		LCD_Fill(60,60,80,80,BLUE);
//		LCD_ShowxNum(70,70,4294967296,10,12,0);
		
			
		//timerTest();
		//feedIwdg();
  }
 

}
