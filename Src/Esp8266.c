/*
*esp8266模块需要连接rst,这里直接开机检测，不再运行中检测
*/

#include "Esp8266.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include <stdarg.h>
extern UART_HandleTypeDef huart2;//串口2
Usart2_Fram espRxFram = {0};
/* 私有函数原形 --------------------------------------------------------------*/
static char * itoa( int value, char * string, int radix );
/*
funName	:espPrintf
input		:
output	:
describe:串口2打印数据
remark	:
*/
void espPrintf(char *Data,...){
const char *s;
	int d;   
	char buf[16];
	uint8_t txdata;
  
	va_list ap;
	va_start(ap, Data);
	while ( * Data != 0 )     // 判断是否到达字符串结束符
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
          txdata=0x0d;
					HAL_UART_Transmit(&huart2,&txdata,1,0xFF);
					Data ++;
				break;
				case 'n':							          //换行符
          txdata=0x0a;
					HAL_UART_Transmit(&huart2,&txdata,1,0xFF);
					Data ++;
				break;
				default:
					Data ++;
				break;
			}			 
		}
		else if ( * Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
					for ( ; *s; s++) 
					{
				  	HAL_UART_Transmit(&huart2,(uint8_t *)s,1,0xFF);
            while(__HAL_UART_GET_FLAG(&huart2,USART_FLAG_TXE)==0);
					}				
					Data++;				
				break;
				case 'd':			
					//十进制
					d = va_arg(ap, int);					
					itoa(d, buf, 10);					
					for (s = buf; *s; s++) 
					{
						HAL_UART_Transmit(&huart2,(uint8_t *)s,1,0xFF);
						while(__HAL_UART_GET_FLAG(&huart2,USART_FLAG_TXE)==0);
					}					
					Data++;				
				break;				
				default:
					Data++;				
				break;				
			}		 
		}		
		else
    {
      HAL_UART_Transmit(&huart2,(uint8_t *)Data,1,0xFF);
      Data++;
		  while(__HAL_UART_GET_FLAG(&huart2,USART_FLAG_TXE)==0);
    }
	}
}
/**
  * 函数功能: 将整形数据转换成字符串
  * 输入参数: radix =10 表示10进制，其他结果为0
  *           value 要转换的整形数
  *           buf 转换后的字符串
  *           radix = 10
  * 返 回 值: 无
  * 说    明：被espPrintf()调用
  */
static char * itoa( int value, char *string, int radix )
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}
	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';
		/* Make the value positive. */
		value *= -1;
	}
	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;
		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}
	/* Null terminate the string. */
	*ptr = 0;
	return string;
} /* NCL_Itoa */
/*
funName	:espCmd
input		:
char * cmd, AT指令
char * reply1,想要的结果1 
char * reply2, 想要的结果2
uint32_t waittime等待时间
output	:1，指令发送成功0，指令发送失败
describe:esp8266指令
remark	:reply1，reply2为NULL表不需响应，两者为或逻辑关系
*/
bool espCmd(char * cmd, char * reply1, char * reply2, uint32_t waittime){
	espRxFram.InfBit.FramLength = 0;
	espPrintf("%s\r\n",cmd);
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	HAL_Delay( waittime );
	espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength] = '\0';
	printf("%s",espRxFram.Data_RX_BUF);
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( espRxFram .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( espRxFram .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( espRxFram .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( espRxFram .Data_RX_BUF, reply2 ) );
}
/*
funName	:espCheck
input		:
output	:1，指令发送成功0，指令发送失败
describe:esp8266检测该模块是否存在
remark	:
*/
bool espCheck(void){
	uint8_t count = 10;
	while(count){
		if(espCmd("AT","OK",NULL,1000)){
			return 1;
		}
		HAL_Delay(1000);
		count--;
	}
	return 0;
}
/*
funName	:espChooseMode
input		:mode 分为三种模式
output	:1，指令发送成功0，指令发送失败
describe:模式设置
remark	:
*/
bool espChooseMode(modeTypeDef mode){
	bool result = 0;
	uint8_t count = 10;
	while(count){
		switch(mode){
			case STA:
				result=espCmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
			break;
			case AP:
				result=espCmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
			break;
			case STA_AP:
				result=espCmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
			break;
			default:
				result=false;
			break;
		}
		if(result)return 1;
		count--;
	}
	return 0;
}
/*
funName	:espJoinAP
input		:pSSID wifi名，pPassWord 密码
output	:1，指令发送成功0，指令发送失败
describe:连接wifi
remark	:
*/
bool espJoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];
	char count=0;
	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	while(count<10)
	{
		if(espCmd(cCmd,"OK",NULL,5000))return 1;
		++count;
	}
	return 0;	
}
/*
funName	:espEnableMultipleId
input		:enumEnUnvarnishTx，配置是否多连接
output	:1，指令发送成功0，指令发送失败
describe:ESP8266模块启动多连接
remark	:
*/
bool espEnableMultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	char count=0;
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	while(count<10)
	{
		if(espCmd(cStr,"OK",0,500))return 1;
		++count;
	}
	return 0;		
}
/*
funName	:espLinkServer
input		:
enumE，网络协议
ip，服务器IP字符串
ComNum，服务器端口字符串
id，模块连接服务器的ID
output	:1，指令发送成功0，指令发送失败
describe:ESP8266模块连接外部服务器
remark	:
*/
bool espLinkServer ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return espCmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );
	
}
/*
funName	:espUnvarnishSend
input		:
output	:1，指令发送成功0，指令发送失败
describe:ESP8266模块启动透传模式
remark	:
*/
bool espUnvarnishSend ( void )
{
	if ( ! espCmd ( "AT+CIPMODE=1", "OK", 0, 1000 ) )
		return false;
	return  espCmd ( "AT+CIPSEND", "OK", ">", 1000 );
}
/*
funName	:espStart
input		:
output	:0-5,0成功，1-5均失败
describe:ESP8266模块启动
remark	:
*/
uint8_t espStart(void){
	uint8_t count = 5;
	if(!espCheck()){
		printf("没有检测到esp8266！\n");
		return 1;
	}
	if(!espChooseMode(STA)){
		printf("模式设置失败！\n");
		return 2;
	}
	if(!espJoinAP(WIFI_ID,WIFI_KEY)){
		printf("wifi设置失败！\n");
		return 3;
	}
	espEnableMultipleId(DISABLE);
	while(count){
		if(espLinkServer(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0)){
			break;
		}
		else{
			count --;
		}
		if(count == 0){
			printf("连接服务器失败\n");
			return 4;
		}
	}
	count = 5;
	while(count){
		if(espUnvarnishSend()){
			break;
		}else{
			count --;
		}
		if(count == 0){
			printf("透传模式设置失败！\n");
			return 5;
		}
	}
	printf("配置完毕！\n");
	return 0;
}
/*
funName	:espSendString
input		:
enumEnUnvarnishTx，声明是否已使能了透传模式
pStr，要发送的字符串
ucId，哪个ID发送的字符串
ulStrLength，要发送的字符串的字节数
output	:1，指令发送成功0，指令发送失败
describe:透传模式发送数据
remark	:
*/
bool espSendString ( FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	
		
	if ( enumEnUnvarnishTx )
	{
		espPrintf ( "%s", pStr );
		
		bRet = true;
		
	}

	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		espCmd ( cStr, "> ", 0, 1000 );

		bRet = espCmd ( pStr, "SEND OK", 0, 1000 );
  }
	return bRet;
}
/*
funName	:espSend
input		:
pStr，要发送的字符串
output	:1，指令发送成功0，指令发送失败
describe:透传模式发送数据
remark	:
*/
bool espSend(char * pStr){
	return espSendString(ENABLE,pStr,0,Single_ID_0);
}
/*
funName	:espExitUnvarnishSend
input		:
output	:
describe:退出透传模式
remark	:
*/
void espExitUnvarnishSend ( void )
{
	HAL_Delay(1000);	
	espPrintf("+++");
	HAL_Delay(500); 
	
}
/*
funName	:espGetLinkStatus
input		:
output	:
2，获得ip
3，建立连接
4，失去连接
0，获取状态失败
describe:获取ESP8266 的连接状态，较适合单端口时使用
remark	:
*/
uint8_t espGetLinkStatus ( void )
{
	if ( espCmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
	{
		if ( strstr (  espRxFram.Data_RX_BUF, "STATUS:2\r\n" ) )
			return 2;
		
		else if ( strstr ( espRxFram .Data_RX_BUF, "STATUS:3\r\n" ) )
			return 3;
		
		else if ( strstr ( espRxFram .Data_RX_BUF, "STATUS:4\r\n" ) )
			return 4;		
	}
	return 0;
}
void espScanConnect(void){
	uint8_t stuts;
	uint8_t count = 5;
	if(machineState.espCloseFlag){
		espExitUnvarnishSend();
		do{
			stuts = espGetLinkStatus();
		}while(!stuts);
		if(stuts == 4){
			printf("正在重连热点和服务器 ......\n");				
			if(!espJoinAP(WIFI_ID,WIFI_KEY)){
				printf("wifi设置失败！\n");
			}	
			while(count){
				if(espLinkServer(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0)){
					break;
				}
				else{
					count --;
				}
				if(count == 0){
					printf("连接服务器失败\n");
				}
			}				
			printf("重连热点和服务器成功!!!\n");
		}
		count = 5;
		while(count){
			if(espUnvarnishSend()){
				break;
			}else{
				count --;
			}
			if(count == 0){
				printf("透传模式设置失败！\n");
			}
		}
	}
}
