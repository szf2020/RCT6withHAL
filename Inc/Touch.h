#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "stm32f1xx_hal.h"
#include "lcd.h"
#include "Iic.h"

#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了 
#define CT_MAX_TOUCH  5    //电容屏支持的点数,固定为5点

//#define CMD_RDX 208
//#define CMD_RDY 144

#define TP_MOSI_HIGH()                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET)
#define TP_MOSI_LOW()                     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET)
#define TP_SCK_HIGH()                    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET)
#define TP_SCK_LOW()                     	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET)
#define TP_CS_HIGH()                    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
#define TP_CS_LOW()                     	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)

#define IS_TP_PEN_HIGH									(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1) == GPIO_PIN_SET)
#define IS_TP_PEN_LOW										(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1) == GPIO_PIN_RESET)
#define IS_TP_MISO_HIGH									(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2) == GPIO_PIN_SET)
#define IS_TP_MISO_LOW									(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2) == GPIO_PIN_RESET)

//触摸屏控制器
typedef struct
{
	void (*init)(void);			//初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	uint8_t  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
}TOUCH_DEV;
extern TOUCH_DEV touchDev;
void initTouch(void);
uint8_t scanTouch(uint8_t);
void adjustTouch(void);
uint8_t touchGetAdjData(void);
void touchSaveAdjData(void);

#endif
