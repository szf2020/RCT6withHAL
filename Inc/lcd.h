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

//#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //Ƭѡ�˿�  		PC9
//#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //����/���� 		PC8	   
//#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //д����			PC7
//#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //������			PC6
//								    
//#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //Ƭѡ�˿�  		PC9
//#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //����/����		PC8	   
//#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //д����			PC7
//#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //������			PC6   

#define DATAOUT(x) GPIOB->ODR=x; //�������
#define DATAIN     GPIOB->IDR;   //��������	

//д���ݺ���
//#define lcdRegData(data){\
//LCD_RS_SET;\
//LCD_CS_CLR;\
//DATAOUT(data);\
//LCD_WR_CLR;\
//LCD_WR_SET;\
//LCD_CS_SET;\
//} 
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���

//������ɫ
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
#define BROWN 			 			0XBC40 //��ɫ
#define BRRED 			 			0XFC07 //�غ�ɫ
#define GRAY  			 			0X8430 //��ɫ

typedef struct{
	uint16_t width;//lcd���
	uint16_t height;//lcd�߶�
	uint16_t id;//lcd id
	uint8_t  dir;//������0:����1:����
	uint16_t gramCmd;//��ʼдgramָ��
	uint16_t setXCmd;//����x����ָ��
	uint16_t setYCmd;//����y����ָ��
}LCD_DEVICE;

extern uint16_t POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern uint16_t BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

void initTFTLCD(void);
void LCD_Clear(uint16_t color);
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);
void LCD_DrawPoint(uint16_t x,uint16_t y);
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void setLcdDir(uint8_t dir);
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);
//void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);

#endif
