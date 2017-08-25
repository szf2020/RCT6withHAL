#include "OneNET.h"
HttpProtocol httpProtocol = {0};
OneNetHttp oneNetHttp = {0};
JsonRegisterDe jsonReg = {0};
uint16_t countContentSize(char *content){
	uint16_t i;
	for(i = 0;i < CONTENT_MAX_SIZE;i++){
		if(content[i] == '}'){
			break;
		}
	}
	return i+1;
}
BOOLEAN searchStr(char *source,char *str,char *save){
	uint16_t sourceCount = 0;
	uint8_t strCount = 0;
	uint8_t saveCount = 0;
	while(source[sourceCount] != '{'){
		sourceCount ++;
	}
	while(1){
		if(source[sourceCount] == str[strCount]){
			sourceCount ++;
			strCount ++;
			if(str[strCount] == '\0'){
				break;
			}
			
		}else{
			strCount = 0;
			sourceCount ++;
			if(source[sourceCount] == '\0'){
				save[saveCount] = '\0';
				return FALSE;
			}
			
		}
	}
	sourceCount = sourceCount + 2;
	while(source[sourceCount] != ','){
		if(source[sourceCount] == '}')break;
		save[saveCount] = source[sourceCount];
		saveCount ++;
		sourceCount ++;
	}
	save[saveCount] = '\0';
	return TRUE;
}
void sendHttp(void){
	printf("%s",oneNetHttp.protocol);
	printf("%s",oneNetHttp.head);
	printf("%s",oneNetHttp.host);
	printf("%s",oneNetHttp.contentLength);
	printf("%s\n",oneNetHttp.json);
	
	espPrintf("%s",oneNetHttp.protocol);
	espPrintf("%s",oneNetHttp.head);
	espPrintf("%s",oneNetHttp.host);
	espPrintf("%s",oneNetHttp.contentLength);
	espPrintf("%s",oneNetHttp.json);
}
void recieveHttp(void){
	HAL_Delay(2000);
	espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength] = '\0';
	printf("%s\n",espRxFram.Data_RX_BUF);
}
JsonRegisterDe registerDe(char *sn,char *title){
	
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "POST";
	httpProtocol.url = "/register_de?register_code=4s6bBQQb1imhUDQv";
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = NOTHING;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"sn\":\"%s\",\"title\":\"%s\"}",sn,title);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	searchStr(espRxFram.Data_RX_BUF,"errno",jsonReg.err.errno);
	if(jsonReg.err.errno[0] == '0'){
		searchStr(espRxFram.Data_RX_BUF,"device_id",jsonReg.device_id);
		searchStr(espRxFram.Data_RX_BUF,"key",jsonReg.key);
	}
	searchStr(espRxFram.Data_RX_BUF,"error",jsonReg.err.error);
	
	printf("errno = %s\n",jsonReg.err.errno);
	if(jsonReg.err.errno[0] == '0'){
		printf("device_id = %s\n",jsonReg.device_id);
		printf("key = %s\n",jsonReg.key);
	}
	printf("error = %s\n",jsonReg.err.error);
	return jsonReg;
}

JsonErr registerAttr(char *allow_dup){
	JsonErr jsonErr = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "PUT";
	httpProtocol.url = "/register_attr?";
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"allow_dup\":%s}",allow_dup);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	searchStr(espRxFram.Data_RX_BUF,"errno",jsonErr.errno);
	searchStr(espRxFram.Data_RX_BUF,"error",jsonErr.error);
	
	printf("errno = %s\n",jsonErr.errno);
	printf("error = %s\n",jsonErr.error);
	return jsonErr;
}
void getDevice(char *deviceId){
	char url[100];
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "GET";
	httpProtocol.url = "http://api.heclouds.com/devices/";
	sprintf(url,"%s%s",httpProtocol.url,deviceId);
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = "Host:api.heclouds.com\r\n\r\n";
	printf("%s",oneNetHttp.protocol);
	printf("%s",oneNetHttp.head);
	printf("%s\n",oneNetHttp.host);
	
	espPrintf("%s",oneNetHttp.protocol);
	espPrintf("%s",oneNetHttp.head);
	espPrintf("%s",oneNetHttp.host);
	recieveHttp();
}
