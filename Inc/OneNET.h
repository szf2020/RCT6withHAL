#ifndef _ONENET_H_
#define _ONENET_H_

#include "Esp8266.h"
#include "string.h"


//OneNET 相关http协议
#define NOTHING																		""
#define APIKEY_HEAD																"api-key: VJjx0ugy6uTIVVNC2d16Un=elPs=\r\n"
#define HOST																			"Host:api.heclouds.com\r\n"
#define CONTENT_LEN																"Content-Length:"

#define CONTENT_MAX_SIZE													1024
#pragma pack(1)
typedef struct{
	uint16_t jsonSize;
	char *httpType;
	char url[CONTENT_MAX_SIZE];
}HttpProtocol;

typedef struct{
	char protocol[CONTENT_MAX_SIZE];
	char *head;
	char *host;
	char contentLength[25];
	char json[CONTENT_MAX_SIZE];
}OneNetHttp;

typedef struct{
	char errno[3];
	char error[7];
}JsonErr;

typedef struct{
	JsonErr err;
	char key[31];
	char device_id[11];
}JsonRegisterDe;

typedef struct{
	JsonErr err;
	
	char ds_uuid[40];
	char id[25];
	char unit_symbol[10];
	char current_value[10];
}JsonDataStreams;

typedef struct{
	char id[9];
	char online[6];
	char title[25];
	char privated[6];
	char key[30];
	char auth_info[25];
	char ds_uuid[40];
	char dataStreamsId[25];
	char unit_symbol[10];
}JsonDeviceAttr;

typedef struct{
	JsonDeviceAttr jsonDeviceAttr;
	JsonErr err;
}JsonGetDevice;

#pragma pack()

JsonRegisterDe 	registerDe(char *sn,char *title);
JsonErr 				registerAttr(char *allow_dup);

JsonGetDevice 	getDevice(char *deviceId);
JsonRegisterDe 	incDevice(char *title,char *privated,char *auth_info);
JsonErr 				updateDevice(char *deviceId,char *title,char *privated,char *auth_info);
JsonErr 				deleteDevice(char *deviceId);

JsonDataStreams incDataStreams(char *deviceId,char *id,char *unit_symbol);
JsonErr 				updateDataStreams(char *deviceId,char *dataStreamsId,char *unit_symbol);
JsonDataStreams getDataStreams(char *deviceId,char *dataStreamsId);
JsonErr deleteDataStreams(char *deviceId,char *dataStreamsId);
#endif
