#include "OneNET.h"
#include "stdlib.h"
HttpProtocol httpProtocol = {0};
OneNetHttp oneNetHttp = {0};
JsonRegisterDe jsonReg = {0};
/*
funName	:countContentSize
input		:content内容
output	:uint16_t 大小
describe:计算要发送的内容大小
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
source要查找的内容
str：键
save:保存对应的值
output	:uint16_t 大小
describe:查找键对应的值
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
describe:发送http到onenet服务器
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
describe:接收http返回的数据
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
sn:设备编号
title:设备名称
output	:JsonRegisterDe:返回设备属性
describe:注册设备
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
allow_dup：是否允许重复注册
output	:JsonErr:返回错误列表
describe:设备属性设置
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
input		:deviceId：设备id
output	:
describe:获取设备数据
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
	
	printf("%d  dataStreamsId = %s\n",searchStr(espRxFram.Data_RX_BUF,"{\"id\"",jsonGetDevice.jsonDeviceAttr.dataStreamsId),jsonGetDevice.jsonDeviceAttr.dataStreamsId);
	printf("%d  unit_symbol = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"unit_symbol\"",jsonGetDevice.jsonDeviceAttr.unit_symbol),jsonGetDevice.jsonDeviceAttr.unit_symbol);
	printf("%d  ds_uuid = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"ds_uuid\"",jsonGetDevice.jsonDeviceAttr.ds_uuid),jsonGetDevice.jsonDeviceAttr.ds_uuid);
	return jsonGetDevice;
}
/*
funName	:incDevice
input		:
title:设备名称
privated:私有性
auth_info:设备唯一编号
output	:JsonRegisterDe:返回设备属性
describe:新增设备
remark	:比 registerDe 设置的多点
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
deviceId：设备ID
title:设备名称
privated:私有性
auth_info:设备唯一编号
output	:JsonErr
describe:更新设备信息
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
deviceId：设备ID
output	:JsonErr
describe:删除设备
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
deviceId：设备ID
id：数据流名称
unit_symbol:数据流单位符号
output	:JsonDataStreams
describe:设备添加数据流
remark	:
*/
JsonDataStreams incDataStreams(char *deviceId,char *dataStreamsId,char *unit_symbol){
	JsonDataStreams jsonDataStreams = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "POST";
	sprintf(httpProtocol.url,"/devices/%s/datastreams?",deviceId);
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"id\":\"%s\",\"unit_symbol\":\"%s\"}",dataStreamsId,unit_symbol);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonDataStreams.err.errno),jsonDataStreams.err.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonDataStreams.err.error),jsonDataStreams.err.error);
	printf("%d  ds_uuid = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"ds_uuid\"",jsonDataStreams.ds_uuid),jsonDataStreams.ds_uuid);
	return jsonDataStreams;
}
/*
funName	:updateDataStreams
input		:
deviceId：设备ID
id：数据流名称
unit_symbol:数据流单位符号
output	:JsonDataStreams
describe:更新数据流
remark	:
*/
JsonErr updateDataStreams(char *deviceId,char *dataStreamsId,char *unit_symbol){
	JsonErr jsonErr = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "PUT";
	sprintf(httpProtocol.url,"/devices/%s/datastreams/%s?",deviceId,dataStreamsId);
	sprintf(oneNetHttp.protocol,"%s %s HTTP/1.1\r\n",httpProtocol.httpType,httpProtocol.url);
	oneNetHttp.head = APIKEY_HEAD;
	oneNetHttp.host = HOST;
	sprintf(oneNetHttp.json,"{\"unit_symbol\":\"%s\"}",unit_symbol);
	httpProtocol.jsonSize = countContentSize(oneNetHttp.json);
	sprintf(oneNetHttp.contentLength,"Content-Length:%d\r\n\r\n",httpProtocol.jsonSize);
	
	sendHttp();
	recieveHttp();
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonErr.errno),jsonErr.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonErr.error),jsonErr.error);
	return jsonErr;
}
/*
funName	:getDataStreams
input		:
deviceId：设备ID
id：数据流名称
output	:JsonDataStreams
describe:获取数据流
remark	:没有数据流则不返回 current_value 数据
*/
JsonDataStreams getDataStreams(char *deviceId,char *dataStreamsId){
	JsonDataStreams jsonDataStreams = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "GET";
	sprintf(httpProtocol.url,"/devices/%s/datastreams/%s?",deviceId,dataStreamsId);
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
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",jsonDataStreams.err.errno),jsonDataStreams.err.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",jsonDataStreams.err.error),jsonDataStreams.err.error);
	printf("%d  current_value = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"current_value\"",jsonDataStreams.current_value),jsonDataStreams.current_value);
	return jsonDataStreams;
}
JsonErr deleteDataStreams(char *deviceId,char *dataStreamsId){
	JsonErr JsonErr = {0};
	espRxFram.InfBit.FramLength = 0;
	
	httpProtocol.httpType = "DELETE";
	sprintf(httpProtocol.url,"/devices/%s/datastreams/%s?",deviceId,dataStreamsId);
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
	
	printf("%d  errno = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"errno\"",JsonErr.errno),JsonErr.errno);
	printf("%d  error = %s\n",searchStr(espRxFram.Data_RX_BUF,"\"error\"",JsonErr.error),JsonErr.error);
	return JsonErr;
}
