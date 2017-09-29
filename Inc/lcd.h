#ifndef __LCD_H
#define __LCD_H			 
#include "stm32f1xx_hal.h"


#define LCD_RD_CLR HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET)
#define LCD_RD_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET)

#define LCD_WR_CLR HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET)
#define LCD_WR_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET)

#define LCD_RS_CLR HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET)
#define LCD_RS_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET)

#define LCD_CS_CLR HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET)
#define LCD_CS_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET)

#define LCD_BL_CLR HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET)
#define LCD_BL_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET)

//#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //片选端口  		PC9
//#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //数据/命令 		PC8	   
//#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //写数据			PC7
//#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //读数据			PC6
//								    
//#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //片选端口  		PC9
//#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //数据/命令		PC8	   
//#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //写数据			PC7
//#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //读数据			PC6   

#define DATAOUT(x) GPIOB->ODR=x; //数据输出
#define DATAIN     GPIOB->IDR;   //数据输入	

//写数据函数
//#define lcdRegData(data){\
//LCD_RS_SET;\
//LCD_CS_CLR;\
//DATAOUT(data);\
//LCD_WR_CLR;\
//LCD_WR_SET;\
//LCD_CS_SET;\
//} 
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

//画笔颜色
#define WHITE							0xFFFF
#define BLACK         	 	0x0000	  
#define BLUE         	 		0x001F  
#define BRED             	0XF81F
#define GRED 							0XFFE0
#define GBLUE			 				0X07FF
#define RED           	 	0xF800
#define MAGENTA       	 	0xF81F
#define GREEN         	 	0x07E0
#define CYAN          	 	0x7FFF
#define YELLOW        	 	0xFFE0
#define BROWN 			 			0XBC40 //棕色
#define BRRED 			 			0XFC07 //棕红色
#define GRAY  			 			0X8430 //灰色

typedef struct{
	uint16_t width;//lcd宽度
	uint16_t height;//lcd高度
	uint16_t id;//lcd id
	uint8_t  dir;//横竖屏0:竖屏1:横屏
	uint16_t gramCmd;//开始写gram指令
	uint16_t setXCmd;//设置x坐标指令
	uint16_t setYCmd;//设置y坐标指令
}LCD_DEVICE;

extern uint16_t POINT_COLOR;//默认红色    
extern uint16_t BACK_COLOR; //背景颜色.默认为白色
extern LCD_DEVICE lcdDevice;

void initTFTLCD(void);
void LCD_Clear(uint16_t color);
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);
void LCD_DrawPoint(uint16_t x,uint16_t y);
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
void setLcdDir(uint8_t dir);
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);
//void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);

#endif
