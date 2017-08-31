/*
*esp8266ģ����Ҫ����rst,����ֱ�ӿ�����⣬���������м��
*/

#include "Esp8266.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include <stdarg.h>
#include "Timer.h"
#include "Iic.h"
extern UART_HandleTypeDef huart2;//����2
Usart2_Fram espRxFram = {0};
EspWifi espWifi = {0};
/* ˽�к���ԭ�� --------------------------------------------------------------*/
static char * itoa( int value, char * string, int radix );
/*
funName	:espPrintf
input		:
output	:
describe:����2��ӡ����
remark	:
*/
void espPrintf(char *Data,...){
const char *s;
	int d;   
	char buf[16];
	uint8_t txdata;
  
	va_list ap;
	va_start(ap, Data);
	while ( * Data != 0 )     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
          txdata=0x0d;
					HAL_UART_Transmit(&huart2,&txdata,1,0xFF);
					Data ++;
				break;
				case 'n':							          //���з�
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
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
					for ( ; *s; s++) 
					{
				  	HAL_UART_Transmit(&huart2,(uint8_t *)s,1,0xFF);
            while(__HAL_UART_GET_FLAG(&huart2,USART_FLAG_TXE)==0);
					}				
					Data++;				
				break;
				case 'd':			
					//ʮ����
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
  * ��������: ����������ת�����ַ���
  * �������: radix =10 ��ʾ10���ƣ��������Ϊ0
  *           value Ҫת����������
  *           buf ת������ַ���
  *           radix = 10
  * �� �� ֵ: ��
  * ˵    ������espPrintf()����
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
char * cmd, ATָ��
char * reply1,��Ҫ�Ľ��1 
char * reply2, ��Ҫ�Ľ��2
uint32_t waittime�ȴ�ʱ��
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:esp8266ָ��
remark	:reply1��reply2ΪNULL������Ӧ������Ϊ���߼���ϵ
*/
bool espCmd(char * cmd, char * reply1, char * reply2, uint32_t waittime){
	espRxFram.InfBit.FramLength = 0;
	espPrintf("%s\r\n",cmd);
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
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
funName	:espAPScan
input		:
output	:
describe:wifi�ȵ��������
remark	:
*/
bool espAPScan(void){
	
	uint8_t read[WIFI_SIZE];
	char *p;
	uint8_t size;
	int i;
	
	eepromReadBytes(read, WIFI_NAME_ADR, WIFI_SIZE);
	printf("%s\n",read);
	if(strstr((char *)read,"wifi:")?1:0){
		for(i = 0;read[i] != 0;i++){
			espWifi.wifiName[i] = read[i + 5];
		}
		espWifi.wifiName[i] = '\0';
		printf("%s\n",espWifi.wifiName);
	}else{
		printf("no wifi name\n");
	}
	eepromReadBytes(read, WIFI_KEY_ADR, WIFI_SIZE);
	printf("%s\n",read);
	if(strstr((char *)read,"key:")?1:0){
		for(i = 0;read[i] != 0;i++){
			espWifi.wifiKey[i] = read[i + 4];
		}
		espWifi.wifiKey[i] = '\0';
		printf("%s\n",espWifi.wifiKey);
		return false;
	}else{
		printf("no wifi key\n");
	}
	/*------------------------------------------------------------------------------------*/
	while(1){
		if(timercounter[2] % 500 == 0){
			espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength] = '\0';
			espRxFram.InfBit.FramLength = 0;
			if(strstr(espRxFram.Data_RX_BUF,"+IPD")?1:0){
				printf("%s\n",espRxFram.Data_RX_BUF);
				if((p = strstr(espRxFram.Data_RX_BUF,"wifi:"))?1:0){
					sprintf(espWifi.wifiName,"%s",p);
					size = strlen(espWifi.wifiName);
					printf("%s---%d\n",espWifi.wifiName,size + 1);
					eepromWriteBytes((uint8_t *)espWifi.wifiName,WIFI_NAME_ADR,size + 1);
					HAL_Delay(100);
					eepromReadBytes(read, WIFI_NAME_ADR, size + 1);
					for (i=0;i<size + 1;i++)
					{    
						printf("%02X ", read[i]);
					}
				}
				if((p = strstr(espRxFram.Data_RX_BUF,"key:"))?1:0){
					sprintf(espWifi.wifiKey,"%s",p);
					size = strlen(espWifi.wifiKey);
					printf("%s---%d\n",espWifi.wifiKey,size + 1);
					eepromWriteBytes((uint8_t *)espWifi.wifiKey,WIFI_KEY_ADR,size + 1);
					HAL_Delay(100);
					eepromReadBytes(read, WIFI_KEY_ADR, size + 1);
					for (i=0;i<size + 1;i++)
					{    
						printf("%02X ", read[i]);
					}
					 return true;
				}
			}
		}
	}
}
/*
funName	:espCheck
input		:
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:esp8266����ģ���Ƿ����
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
input		:mode ��Ϊ����ģʽ
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:ģʽ����
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
input		:pSSID wifi����pPassWord ����
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:����wifi
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
input		:enumEnUnvarnishTx�������Ƿ������
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:ESP8266ģ������������
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
funName	:espEnableServer
input		:enumEnUnvarnishTx���Ƿ�����Ϊ������
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:ESP8266ģ����Ϊ������
remark	:
*/
bool espEnableServer( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	char count=0;
	if(enumEnUnvarnishTx == ENABLE){
		sprintf ( cStr, "AT+CIPSERVER=1,5000");
	}else{
		sprintf ( cStr, "AT+CIPSERVER=0");
	}
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
enumE������Э��
ip��������IP�ַ���
ComNum���������˿��ַ���
id��ģ�����ӷ�������ID
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:ESP8266ģ�������ⲿ������
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
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:ESP8266ģ������͸��ģʽ
remark	:
*/
bool espUnvarnishSend ( void )
{
	if ( ! espCmd ( "AT+CIPMODE=1", "OK", 0, 1000 ) )
		return false;
	return  espCmd ( "AT+CIPSEND", "OK", ">", 1000 );
}
/*
funName	:espServerMode
input		:
output	:
describe:ESP8266���÷�����ģʽ
remark	:���տͻ�����Ϣ
*/
void espServerMode(void){
	if(!espCheck()){
		printf("û�м�⵽esp8266��\n");
		while(1);
	}
	if(!espChooseMode(STA_AP)){
		printf("ģʽ����ʧ�ܣ�\n");
		while(1);
	}
	espEnableMultipleId(ENABLE);
	espEnableServer(ENABLE);
}
/*
funName	:espStart
input		:
output	:
describe:ESP8266����wifi
remark	:
*/
void espStart(void){
	uint8_t count = 5;
	espEnableServer(DISABLE);
	espEnableMultipleId(DISABLE);
	if(!espJoinAP(espWifi.wifiName,espWifi.wifiKey)){
		printf("wifi����ʧ�ܣ�\n");
		while(1);
	}

	while(count){
		if(espLinkServer(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0)){
			break;
		}
		else{
			count --;
		}
		if(count == 0){
			printf("���ӷ�����ʧ��\n");
			while(1);
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
			printf("͸��ģʽ����ʧ�ܣ�\n");
			while(1);
		}
	}
}
/*
funName	:espSendString
input		:
enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
pStr��Ҫ���͵��ַ���
ucId���ĸ�ID���͵��ַ���
ulStrLength��Ҫ���͵��ַ������ֽ���
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:͸��ģʽ��������
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
pStr��Ҫ���͵��ַ���
output	:1��ָ��ͳɹ�0��ָ���ʧ��
describe:͸��ģʽ��������
remark	:

*/

bool espSend(void){
//	return espSendString(ENABLE,pStr,0,Single_ID_0);
	espRxFram.InfBit.FramLength = 0;
//	char cStr1 [] = {"POST /register_de?register_code=4s6bBQQb1imhUDQv HTTP/1.1"};
//	char cStrn [] = {"\n"};
//	char cStr3 [] = {"Host:api.heclouds.com"};
//	char cStr4 [] = {"Content-Length:33"};
//	char cStr5 [] = {"Connection:close"};
//	char cStr6 [] = {"{\"sn\":\"201708241522\",\"title\":\"test3\"}"};
	espPrintf("POST /register_de?register_code=4s6bBQQb1imhUDQv HTTP/1.1\r\n");
	espPrintf("Host:api.heclouds.com\r\n");
	espPrintf("Content-Length:33\r\n\r\n");
	//espPrintf("Connection:close\r\n\r\n");
	espPrintf("{\"sn\":\"20180808\",\"title\":\"test4\"}");
	HAL_Delay(2000);
	espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength] = '\0';
	printf("%s",espRxFram.Data_RX_BUF);
	return true;
}
/*
funName	:espExitUnvarnishSend
input		:
output	:
describe:�˳�͸��ģʽ
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
2�����ip
3����������
4��ʧȥ����
0����ȡ״̬ʧ��
describe:��ȡESP8266 ������״̬�����ʺϵ��˿�ʱʹ��
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
			printf("���������ȵ�ͷ����� ......\n");				
			if(!espJoinAP(WIFI_ID,WIFI_KEY)){
				printf("wifi����ʧ�ܣ�\n");
			}	
			while(count){
				if(espLinkServer(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0)){
					break;
				}
				else{
					count --;
				}
				if(count == 0){
					printf("���ӷ�����ʧ��\n");
				}
			}				
			printf("�����ȵ�ͷ������ɹ�!!!\n");
		}
		count = 5;
		while(count){
			if(espUnvarnishSend()){
				break;
			}else{
				count --;
			}
			if(count == 0){
				printf("͸��ģʽ����ʧ�ܣ�\n");
			}
		}
	}
}
