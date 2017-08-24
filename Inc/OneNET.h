#ifndef _ONENET_H_
#define _ONENET_H_

#include "Esp8266.h"
#include "string.h"


//OneNET 相关http协议
#define	POST																			"POST"
#define PUT																				"PUT"

#define PARAM_DEV_REG_CODE												"register_code=4s6bBQQb1imhUDQv"
#define HTTP_PROTOCOL															"HTTP/1.1"

#define URL_REG																		"/register_de?"

#define HOST																			"Host:api.heclouds.com\r\n"
#define CONTENT_LEN																"Content-Length:"

#define CONTENT_MAX_SIZE													200

typedef struct{
	char *httpType;
	char *url;
	char *param[15];
	char *httpProtocol;
	char *host;
	char *contentLenStr;
	char *json;
	uint16_t jsonSize;
}OneNET_HTTP;

void registerDevice(char *sn,char *title);

#endif
