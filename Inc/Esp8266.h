#ifndef _ESP8266_H_
#define _ESP8266_H_
#include "stm32f1xx_hal.h"
#include <stdbool.h>
#define RX_BUF_MAX_LEN     1024                                     //�����ջ����ֽ���
#define WIFI_ID 																	"dr.kon"
#define WIFI_KEY 																	"yuanquan_709"

#define User_ESP8266_TcpServer_IP                 "api.heclouds.com"       //Ҫ���ӵķ������� IP
#define User_ESP8266_TcpServer_Port               "80"                 //Ҫ���ӵķ������Ķ˿�
#define PID																				93205

#pragma anon_unions
typedef struct                                    //��������֡�Ĵ���ṹ��
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
  union {
    __IO uint16_t InfAll;
    struct {
		  __IO uint16_t FramLength       :15;                               // 14:0 
		  __IO uint16_t FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 	
}Usart2_Fram;
extern Usart2_Fram espRxFram;

typedef enum{
	STA,
	AP,
	STA_AP
}modeTypeDef;

typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;

uint8_t espStart(void);
void espScanConnect(void);
bool espSend(void);
void espPrintf(char *Data,...);
#endif
