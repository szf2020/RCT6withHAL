#include "OneNET.h"
OneNET_HTTP oneNETHttp = {0};
uint16_t countContentSize(char *content){
	uint16_t i;
	for(i = 0;i < CONTENT_MAX_SIZE;i++){
		if(content[i] == '}'){
			break;
		}
	}
	return i+1;
}
void registerDevice(char *sn,char *title){
	char url[100];
	char content[CONTENT_MAX_SIZE];
	char contentLenStr[20];
	espRxFram.InfBit.FramLength = 0;
	oneNETHttp.httpType = "POST";
	oneNETHttp.url = URL_REG;
	oneNETHttp.param[0] = PARAM_DEV_REG_CODE;
	oneNETHttp.httpProtocol = HTTP_PROTOCOL;
	sprintf(url,"%s %s%s %s\r\n",oneNETHttp.httpType,oneNETHttp.url,oneNETHttp.param[0],oneNETHttp.httpProtocol);
	oneNETHttp.host = HOST;
	sprintf(content,"{\"sn\":\"%s\",\"title\":\"%s\"}",sn,title);
	oneNETHttp.jsonSize = countContentSize(content);
	sprintf(contentLenStr,"%s%d\r\n\r\n",CONTENT_LEN,oneNETHttp.jsonSize);
	oneNETHttp.contentLenStr = contentLenStr;
	printf("%s",url);
	printf("%s",oneNETHttp.host);
	printf("%s",contentLenStr);
	printf("%s\n",content);
	
	espPrintf("%s",url);
	espPrintf("%s",oneNETHttp.host);
	espPrintf("%s",contentLenStr);
	espPrintf("%s",content);
	
	HAL_Delay(2000);
	espRxFram.Data_RX_BUF[espRxFram.InfBit.FramLength] = '\0';
	printf("%s",espRxFram.Data_RX_BUF);
	
}
