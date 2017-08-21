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
#include "usb_device.h"
#include "fatfs.h"
#include "usbd_cdc_if.h"
#include "stdlib.h"
#include "StmFlash.h"
#include "infrared.h"
MACHINE_STATE machineState;
ERROR_STUS errorStus = E_OK;
RTC_TIME rtcTime;
/* 用于保存转换计算后的电压值 */	 
//float Current_Temperature[2];
//uint8_t test[100] = {0};
//uint8_t UserTxBuffer[100] = {1,2,3,4};
uint32_t FlashID = 0;
uint32_t DeviceID = 0;
////FATFS fs;
//FRESULT fatsFlag;
int main(void)
{
	errorStus = setPriorityGroup();
	errorStus = systemClockConfig();
	
	rccClockEnable();
	
	initUart(USART1);
	initLed();
	initButton();
	initTimer(TIM1);
	initTimer(TIM2);//10us
//	initTimer(TIM3);
//	initTimer(TIM4);
//	initTimer(TIM5);
//	initTimer(TIM6);
//	initTimer(TIM7);
//	initTimer(TIM8);
	initTFTLCD();
	initRTC();
	//initADC1();
	errorStus = initIic();
//	if(errorStus == E_OK){
//		iicTest();
//	}
	initSpix(SPI1);
	DeviceID = spiFlashReadDeviceID();
	HAL_Delay(100);
	FlashID = spiFlashReadID();
//	printf("FlashID is 0x%X,  Manufacturer Device ID is 0x%X\n", FlashID, DeviceID);
	spiTest();
	StmFlashTest();
	touchDev.init();
	MX_USB_DEVICE_Init();
	initInfrared();//红外和ADCDMA中的PA1脚是冲突的,两者只能取其1
//	MX_FATFS_Init();
//	fatsFlag = f_mount(&fs,"",0);
//	if(fatsFlag){
//		printf("fats error\n");
//	}else{
//		printf("fats success\n");
//	}
  /* last set */
	//initIwdg();
  setPriority();
	/* pre init set*/
	checkPowerMode();
	//enterSleepMode();
//	POINT_COLOR = RED;
//	LCD_Set_Window(100,100,100,60);
//	POINT_COLOR = BLUE;
//	printf("你\n");
  while (1)
  {
		scanKey1();
		ledPwm();
		touchDev.scan(0);
//		infraredTest();
		
//		printf("%d",machineState.usbFlag);
//		HAL_Delay(250);
		//CDC_Transmit_FS((uint8_t*)&UserTxBuffer, sizeof(UserTxBuffer));
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
