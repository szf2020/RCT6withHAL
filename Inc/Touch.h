#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "stm32f1xx_hal.h"
#include "lcd.h"
#include "Iic.h"

#define TP_PRES_DOWN 0x80  //����������	  
#define TP_CATH_PRES 0x40  //�а��������� 
#define CT_MAX_TOUCH  5    //������֧�ֵĵ���,�̶�Ϊ5��

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

//������������
typedef struct
{
	void (*init)(void);			//��ʼ��������������
	uint8_t (*scan)(uint8_t);				//ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(void);		//������У׼ 
	uint16_t x[CT_MAX_TOUCH]; 		//��ǰ����
	uint16_t y[CT_MAX_TOUCH];		//�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								//x[4],y[4]�洢��һ�ΰ���ʱ������. 
	uint8_t  sta;					//�ʵ�״̬ 
								//b7:����1/�ɿ�0; 
	                            //b6:0,û�а�������;1,�а�������. 
								//b5:����
								//b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
	uint8_t touchtype;
}TOUCH_DEV;
extern TOUCH_DEV touchDev;
void initTouch(void);
uint8_t scanTouch(uint8_t);
void adjustTouch(void);
uint8_t touchGetAdjData(void);
void touchSaveAdjData(void);

#endif
