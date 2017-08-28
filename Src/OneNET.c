#include "OneNET.h"
#include "stdlib.h"
HttpProtocol httpProtocol = {0};
OneNetHttp oneNetHttp = {0};
JsonRegisterDe jsonReg = {0};
/*
funName	:countContentSize
input		:content����
output	:uint16_t ��С
describe:����Ҫ���͵����ݴ�С
remark	:
*/
uint16_t countContentSize(char *content){
	uint16_t i;
	for(i = 0;i < CONTENT_MAX_SIZE;i++){
		if(content[i] == '\0'){
			break;
		}
	}
	return i;
}
/*
funName	:searchStr
input		:
sourceҪ���ҵ�����
str����
save:�����Ӧ��ֵ
output	:uint16_t ��С
describe:���Ҽ���Ӧ��ֵ
remark	:
*/
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
	sourceCount = sourceCount + 1;
	while(source[sourceCount] != ','){
		if(source[sourceCount] == '}')break;
		save[saveCount] = source[sourceCount];
		saveCount ++;
		sourceCount ++;
	}
	save[saveCount] = '\0';
	return TRUE;
}
/*
funName	:sendHttp
input		:
output	:
describe:����http��onenet������
remark	:
*/
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
/*
funName	:recieveHttp
input		:
output	:
describe:����http���ص�����
remark	:
*/
void recieveHttp(void){
	HAL_Delay(2000);
	espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength] = '\0';
	printf("%s\n",espRxFram.Data_RX_BUF);
}
/*
funName	:registerDe
input		:
sn:�豸���
title:�豸����
output	:JsonRegisterDe:�����豸����
describe:ע���豸
remark	:
*/
JsonRegisterDe registerDe(char *sn,char *title){
	
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "POST";
	sprintf(httpProtocol.url,"/register_de?register_code=4s6bBQQb1imhUDQv");
//	httpProtocol.url = "/register_de?register_code=4s6bBQQb1imhUDQv";
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = NOTHING;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"sn\":\"%s\",\"title\":\"%s\"}",sn,title);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonReg.err.errno),jsonReg.err.errno);
	printf("%d  device_id = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"device_id\"",jsonReg.device_id),jsonReg.device_id);
	printf("%d  key = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"key\"",jsonReg.key),jsonReg.key);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonReg.err.error),jsonReg.err.error);
	return jsonReg;
}
/*
funName	:registerAttr
input		:
allow_dup���Ƿ������ظ�ע��
output	:JsonErr:���ش����б�
describe:�豸��������
remark	:
*/
JsonErr registerAttr(char *allow_dup){
	JsonErr jsonErr = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "PUT";
	sprintf(httpProtocol.url,"/register_attr?");
//	httpProtocol.url = "/register_attr?";
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"allow_dup\":%s}",allow_dup);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonErr.errno),jsonErr.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonErr.error),jsonErr.error);
	
	return jsonErr;
}
/*
funName	:getDevice
input		:deviceId���豸id
output	:
describe:��ȡ�豸����
remark	:
*/
JsonGetDevice getDevice(char *deviceId){
	JsonGetDevice jsonGetDevice = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "GET";
	sprintf(httpProtocol.url,"http://api.heclouds.com/devices/%s",deviceId);
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	oneNetHttp.host = (char *)malloc(30);
	sprintf(oneNetHttp.host,"%s\r\n",HOST);
	sprintf(oneNetHttp.json,"");
	sprintf(oneNetHttp.contentLength,"");
	
	sendHttp();
	free(oneNetHttp.host);
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonGetDevice.err.errno),jsonGetDevice.err.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonGetDevice.err.error),jsonGetDevice.err.error);
	printf("%d  id = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"id\"",jsonGetDevice.jsonDeviceAttr.id),jsonGetDevice.jsonDeviceAttr.id);
	printf("%d  key = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"key\"",jsonGetDevice.jsonDeviceAttr.key),jsonGetDevice.jsonDeviceAttr.key);
	printf("%d  online = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"online\"",jsonGetDevice.jsonDeviceAttr.online),jsonGetDevice.jsonDeviceAttr.online);
	printf("%d  private = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"private\"",jsonGetDevice.jsonDeviceAttr.privated),jsonGetDevice.jsonDeviceAttr.privated);
	printf("%d  title = %s\n",searchStr(espRxFram.Data_RX_BUF,",\"title\"",jsonGetDevice.jsonDeviceAttr.title),jsonGetDevice.jsonDeviceAttr.title);
	printf("%d  auth_info = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"auth_info\"",jsonGetDevice.jsonDeviceAttr.title),jsonGetDevice.jsonDeviceAttr.title);
	return jsonGetDevice;
}
/*
funName	:incDevice
input		:
title:�豸����
privated:˽����
auth_info:�豸Ψһ���
output	:JsonRegisterDe:�����豸����
describe:�����豸
remark	:�� registerDe ���õĶ��
*/
JsonRegisterDe incDevice(char *title,char *privated,char *auth_info){
	JsonRegisterDe jsonRegisterDe = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "POST";
	sprintf(httpProtocol.url,"/devices?");
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"title\":\"%s\",\"private\":\"%s\",\"auth_info\":\"%s\"}",title,privated,auth_info);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonRegisterDe.err.errno),jsonRegisterDe.err.errno);
	printf("%d  device_id = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"device_id\"",jsonRegisterDe.device_id),jsonRegisterDe.device_id);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonRegisterDe.err.error),jsonRegisterDe.err.error);
	return jsonRegisterDe;
}
/*
funName	:updateDevice
input		:
deviceId���豸ID
title:�豸����
privated:˽����
auth_info:�豸Ψһ���
output	:JsonErr
describe:�����豸��Ϣ
remark	:
*/
JsonErr updateDevice(char *deviceId,char *title,char *privated,char *auth_info){
	JsonErr jsonErr = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "PUT";
	sprintf(httpProtocol.url,"/devices/%s?",deviceId);
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"title\":\"%s\",\"private\":\"%s\",\"auth_info\":\"%s\"}",title,privated,auth_info);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonErr.errno),jsonErr.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonErr.error),jsonErr.error);
	return jsonErr;
}
/*
funName	:deleteDevice
input		:
deviceId���豸ID
output	:JsonErr
describe:ɾ���豸
remark	:
*/
JsonErr deleteDevice(char *deviceId){
	JsonErr jsonErr = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "DELETE";
	sprintf(httpProtocol.url,"/devices/%s?",deviceId);
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"");
	sprintf(oneNetHttp.contentLength,"\r\n");
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonErr.errno),jsonErr.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonErr.error),jsonErr.error);
	return jsonErr;
}
/*
funName	:incDataStreams
input		:
deviceId���豸ID
id������������
unit_symbol:��������λ����
output	:JsonDataStreams
describe:�豸���������
remark	:
*/
JsonDataStreams incDataStreams(char *deviceId,char *id,char *unit_symbol){
	JsonDataStreams jsonDataStreams = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "POST";
	sprintf(httpProtocol.url,"/devices/%s/datastreams?",deviceId);
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"id\":\"%s\",\"unit_symbol\":\"%s\"}",id,unit_symbol);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonDataStreams.err.errno),jsonDataStreams.err.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonDataStreams.err.error),jsonDataStreams.err.error);
	printf("%d  ds_uuid = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"ds_uuid\"",jsonDataStreams.ds_uuid),jsonDataStreams.ds_uuid);
	return jsonDataStreams;
}
