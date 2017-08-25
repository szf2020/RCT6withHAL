#ifndef _ONENET_H_
#define _ONENET_H_

#include "Esp8266.h"
#include "string.h"


//OneNET 相关http协议
#define NO_HEAD																		""
#define APIKEY_HEAD																"api-key: VJjx0ugy6uTIVVNC2d16Un=elPs=\r\n"
#define HOST																			"Host:api.heclouds.com\r\n"
#define CONTENT_LEN																"Content-Length:"

#define CONTENT_MAX_SIZE													1024

typedef struct{
	uint16_t jsonSize;
	char *httpType;
	char *url;
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


JsonRegisterDe registerDe(char *sn,char *title);
JsonErr registerAttr(char *allow_dup);
#endif
