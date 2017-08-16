#include "Touch.h"
#include "stdlib.h"
#include "math.h"
TOUCH_DEV touchDev = {
	initTouch,
	scanTouch,
	adjustTouch,
	0,0,0,0,0,0,0,0,
};
/*
funName	:touchWriteByte
input		:byte
output	:
describe:������ICд��1byte����
remark	:
*/
void touchWriteByte(uint8_t byte){
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(byte&0x80)TP_MOSI_HIGH();  
		else TP_MOSI_LOW();   
		byte<<=1;    
		TP_SCK_LOW(); 	 
		TP_SCK_HIGH();		//��������Ч	        
	}		 			
}
/*
funName	:touchReadADC
input		:cmd
output	:����������	 
describe:�Ӵ�����IC��ȡadcֵ
remark	:
*/
uint16_t touchReadADC(uint8_t cmd){
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TP_SCK_LOW();		//������ʱ�� 	 
	TP_MOSI_LOW();	//����������
	TP_CS_LOW(); 		//ѡ�д�����IC
	touchWriteByte(cmd);//����������
	HAL_Delay(1);//ADS7846��ת��ʱ���Ϊ6us
	TP_SCK_LOW(); 	     	    
	HAL_Delay(1);
	TP_SCK_HIGH();		//��1��ʱ�ӣ����BUSY	    	    
	TP_SCK_LOW();	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		TP_SCK_LOW();	//�½�����Ч  	    	   
		TP_SCK_HIGH();
		if(IS_TP_MISO_HIGH)Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	TP_CS_HIGH();//�ͷ�Ƭѡ	
	//printf("cmd = %d,Num = %d\n",cmd,Num);
	return(Num);   
}
/*
funName	:touchReadXOrY
input		:xy
output	:����������	 
describe:��ȡһ������ֵ(x����y)
remark	:������ȡREAD_TIMES������,����Щ������������,Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
*/
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
uint16_t touchReadXOrY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=touchReadADC(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
/*
funName	:touchReadXY
input		:x,y��ȡ��������ֵ
output	:0ʧ��1�ɹ�
describe:��ȡx,y����
remark	:��Сֵ��������100.
*/

uint8_t touchReadXY(uint16_t *x,uint16_t *y)
{
	//Ĭ��Ϊtouchtype=0������.
	uint8_t cmdRdx = 0xd0;
	uint8_t cmdRdy = 0x90;
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=touchReadXOrY(cmdRdx);
	ytemp=touchReadXOrY(cmdRdy);	  												   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	//printf("xtemp = %d,ytemp = %d\n",xtemp,ytemp);
	return 1;//�����ɹ�
}
/*
funName	:touchReadXY2
input		:x,y��ȡ��������ֵ
output	:0ʧ��1�ɹ�
describe:����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���ERR_RANGE,��������,����Ϊ������ȷ,�����������.	 
remark	:�ú����ܴ�����׼ȷ��
*/
#define ERR_RANGE 50 //��Χ 
uint8_t touchReadXY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=touchReadXY(&x1,&y1);   
    if(flag==0)return(0);
    flag=touchReadXY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
			//printf("xtemp = %d,ytemp = %d\n",*x,*y);
        return 1;
    }else return 0;	  
}  
/*----------------------------------��LCD�йز���------------------------------------------------------*/
/*
funName	:TouchDrawPoint
input		:x,y,color
output	:
describe:��һ��������
remark	:
*/
void TouchDrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	POINT_COLOR = color;
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//������Ȧ
}
/*
funName	:touchDrawBigPoint
input		:x,y,color
output	:
describe:��һ�����
remark	:
*/
void touchDrawBigPoint(uint16_t x,uint16_t y,uint16_t color){
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);
}
/*
funName	:touchTest
input		:
output	:
describe:�ж���Ļ�Ƿ�У׼��ûУ׼��У׼
remark	:
*/
void touchTest(void){
	if(touchGetAdjData()){
		printf("�Ѿ�У׼!\n");
	}//�Ѿ�У׼
	else			   //δУ׼?
	{ 										    
		LCD_Clear(WHITE);//����
		adjustTouch();  //��ĻУ׼ 
		touchSaveAdjData();	 
	}			
	touchGetAdjData();
}
/*
funName	:initTouch
input		:
output	:
describe:��ʼ��������
remark	:
*/
void initTouch(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	/*
		PC3:T_MOSI
		PC0:T_SCK
		PC13:T_CS
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_0 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	/*
		PC1:T_PEN
		PC2:T_MISO
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	touchReadXY(&touchDev.x[0],&touchDev.y[0]);//��һ�ζ�ȡ��ʼ��	 
	LCD_Clear(YELLOW);//����
	touchTest();
}
/*
funName	:scanTouch
input		:tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
output	:����״̬������1/�ɿ�0
describe:����ɨ��
remark	:
*/
uint8_t scanTouch(uint8_t tp){
	//printf("%d",IS_TP_PEN_LOW);
	if(IS_TP_PEN_LOW)//�а�������
	{
		if(tp)touchReadXY2(&touchDev.x[0],&touchDev.y[0]);//��ȡ��������
		else if(touchReadXY2(&touchDev.x[0],&touchDev.y[0]))//��ȡ��Ļ����
		{
	 		touchDev.x[0]=touchDev.xfac*touchDev.x[0]+touchDev.xoff;//�����ת��Ϊ��Ļ����
			touchDev.y[0]=touchDev.yfac*touchDev.y[0]+touchDev.yoff;  
			printf("x = %d,y = %d\n",touchDev.x[0],touchDev.y[0]);
	 	} 
		if((touchDev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			touchDev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			touchDev.x[4]=touchDev.x[0];//��¼��һ�ΰ���ʱ������
			touchDev.y[4]=touchDev.y[0];  	   			 
		}
		
	}else
	{
		if(touchDev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			touchDev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			touchDev.x[4]=0;
			touchDev.y[4]=0;
			touchDev.x[0]=0xffff;
			touchDev.y[0]=0xffff;
		}	    
	}
	touchDrawBigPoint(touchDev.x[0],touchDev.y[0],RED);
	return touchDev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}
/*
funName	:touchSaveAdjData
input		:
output	:
describe:����У׼���ݵ�eeprom
remark	:
*/
//������EEPROM����ĵ�ַ�����ַ,ռ��13���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
#define SAVE_ADDR_BASE 40
void touchSaveAdjData(void){
	uint32_t temp;			
	uint8_t data[4];	
	//����У�����!		   							  
	temp=touchDev.xfac*100000000;//����xУ������
	data[0] =(uint8_t)temp & 0xFF;
	data[1] =(uint8_t)((temp >> 8) & 0xFF);
	data[2] =(uint8_t)((temp >> 16) & 0xFF);
	data[3] =(uint8_t)((temp >> 24) & 0xFF);
  eepromWriteBytes(data,SAVE_ADDR_BASE,4);  
	//printf("temp = %d,xfac:%d %d %d %d\n",temp,data[0],data[1],data[2],data[3]);	
	temp=touchDev.yfac*100000000;//����yУ������  
	data[0] =(uint8_t)temp & 0xFF;
	data[1] =(uint8_t)((temp >> 8) & 0xFF);
	data[2] =(uint8_t)((temp >> 16) & 0xFF);
	data[3] =(uint8_t)((temp >> 24) & 0xFF);  
  eepromWriteBytes(data,SAVE_ADDR_BASE+4,4);
	//printf("temp = %d,yfac:%d %d %d %d\n",temp,data[0],data[1],data[2],data[3]);
	//����xƫ����
	data[0] =(uint8_t)touchDev.xoff & 0xFF;
	data[1] =(uint8_t)((touchDev.xoff >> 8) & 0xFF);
   eepromWriteBytes(data,SAVE_ADDR_BASE+8,2);	
	// printf("touchDev.xoff = %d,xoff:%d %d\n",touchDev.xoff,data[0],data[1]);	 
	//����yƫ����
	data[0] =(uint8_t)touchDev.yoff & 0xFF;
	data[1] =(uint8_t)((touchDev.yoff >> 8) & 0xFF);
	eepromWriteBytes(data,SAVE_ADDR_BASE+10,2);	
	//printf("touchDev.yoff = %d,yoff:%d %d\n",touchDev.yoff,data[0],data[1]);
	//���津������
	eepromWriteBytes(&(touchDev.touchtype),SAVE_ADDR_BASE+12,1);	
	temp=0X0A;//���У׼����
	data[0] =(uint8_t)temp & 0xFF;
	eepromWriteBytes(data,SAVE_ADDR_BASE+13,1);
}
/*
funName	:touchGetAdjData
input		:
output	:0û��У׼����1��ȡ�ɹ�
describe:��ȡУ׼����
remark	:
*/
uint8_t touchGetAdjData(void){
	uint8_t data[4];
	uint32_t temp;
	eepromReadBytes(data,SAVE_ADDR_BASE+13,1);
	if(data[0] == 0X0A){
		eepromReadBytes(data,SAVE_ADDR_BASE,4);
		temp = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 16);
		touchDev.xfac=(float)temp/100000000;//�õ�xУ׼����
		eepromReadBytes(data,SAVE_ADDR_BASE+4,4);
		temp = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 16);
		touchDev.yfac=(float)temp/100000000;//�õ�yУ׼����
		//�õ�xƫ����
		eepromReadBytes(data,SAVE_ADDR_BASE + 8,2);
		temp = data[0] | (data[1] << 8);
		touchDev.xoff = temp;			   	  
 	    //�õ�yƫ����
		eepromReadBytes(data,SAVE_ADDR_BASE + 10,2);
		temp = data[0] | (data[1] << 8);
		touchDev.yoff = temp;			
		
		eepromReadBytes(data,SAVE_ADDR_BASE + 12,1);
 		touchDev.touchtype=data[0];//��ȡ�������ͱ��
		//printf("touchDev.xfac = %f\n,touchDev.yfac = %f\n,touchDev.xoff = %d\n,touchDev.yoff = %d\n,touchDev.touchtype = %d\n",touchDev.xfac,touchDev.yfac,touchDev.xoff,touchDev.yoff,touchDev.touchtype);
		return 1;
	}
	return 0;
}
//��ʾ�ַ���
const u8* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
//��ʾУ׼���(��������)
/*
funName	:touchAdjInfoShow
input		:
output	:
describe:У����ʾ��Ϣ
remark	:
*/
void touchAdjInfoShow(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcdDevice.width,lcdDevice.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcdDevice.width,lcdDevice.height,16,"y1:");
 	LCD_ShowString(40,180,lcdDevice.width,lcdDevice.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcdDevice.width,lcdDevice.height,16,"y2:");
	LCD_ShowString(40,200,lcdDevice.width,lcdDevice.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcdDevice.width,lcdDevice.height,16,"y3:");
	LCD_ShowString(40,220,lcdDevice.width,lcdDevice.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcdDevice.width,lcdDevice.height,16,"y4:");  
 	LCD_ShowString(40,240,lcdDevice.width,lcdDevice.height,16,"fac is:");     
	LCD_ShowNum(40+24,160,x0,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,160,y0,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,180,x1,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,180,y1,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,200,x2,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,200,y2,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,220,x3,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,220,y3,4,16);	//��ʾ��ֵ
 	LCD_ShowNum(40+56,lcdDevice.width,fac,3,16); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.

}
//У׼�������(0,1)��(2,3)��(0,2)��(1,3),(1,2)��(0,3)���������ľ���
const u8 TP_ADJDIS_TBL[3][4]={{0,1,2,3},{0,2,1,3},{1,2,0,3}};//У׼��������
//������У׼����
//�õ��ĸ�У׼����
/*
funName	:adjustTouch
input		:
output	:
describe:У�鴥����
remark	:
*/
void adjustTouch(void){
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	
	u16 outtime=0; 	  
	LCD_Clear(WHITE);	//���� 	    
	POINT_COLOR=BLUE;	//��ɫ
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//��ʾ��ʾ��Ϣ
	TouchDrawPoint(20,20,RED);//����1 
	touchDev.sta=0;//���������ź� 
	touchDev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
READJ:
		touchDev.scan(1);//ɨ����������
		if((touchDev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			touchDev.sta&=~(1<<6);	//��ǰ����Ѿ����������. 
			pos_temp[cnt][0]=touchDev.x[0];
			pos_temp[cnt][1]=touchDev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TouchDrawPoint(20,20,WHITE);				//�����1 
					TouchDrawPoint(lcdDevice.width-20,20,RED);	//����2
					break;
				case 2:
 					TouchDrawPoint(lcdDevice.width-20,20,WHITE);	//�����2
					TouchDrawPoint(20,lcdDevice.height-20,RED);	//����3
					break;
				case 3:
 					TouchDrawPoint(20,lcdDevice.height-20,WHITE);			//�����3
 					TouchDrawPoint(lcdDevice.width-20,lcdDevice.height-20,RED);	//����4
					break;                                                                                                                                                                                                                                                          
				case 4:	 //ȫ���ĸ����Ѿ��õ�
					for(cnt=0;cnt<3;cnt++)//���������ľ����Ƿ�������Χ�ڣ�
					{ 
						tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][0]-pos_temp[TP_ADJDIS_TBL[cnt][1]][0]);//x1-x2/x1-x3/x2-x3
						tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][1]-pos_temp[TP_ADJDIS_TBL[cnt][1]][1]);//y1-y2/y1-y3/y2-y3
						tem1*=tem1;
						tem2*=tem2;
						d1=sqrt(tem1+tem2);//�õ�����֮��ľ��� 
						tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][0]-pos_temp[TP_ADJDIS_TBL[cnt][3]][0]);//x3-x4/x2-x4/x1-x4
						tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][1]-pos_temp[TP_ADJDIS_TBL[cnt][3]][1]);//y3-y4/y2-y4/y1-y4
						tem1*=tem1;
						tem2*=tem2;
						d2=sqrt(tem1+tem2);//�õ�����֮��ľ���
						fac=(float)d1/d2;
						if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
						{
							cnt=0;
							TouchDrawPoint(lcdDevice.width-20,lcdDevice.height-20,WHITE);	//�����4
							TouchDrawPoint(20,20,RED);									//����1
							touchAdjInfoShow(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
							goto READJ;	//���ϸ�����У׼
						}
					}  
					//��ȷ��
					//������
					touchDev.xfac=(float)(lcdDevice.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					touchDev.xoff=(lcdDevice.width-touchDev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
 					touchDev.yfac=(float)(lcdDevice.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					touchDev.yoff=(lcdDevice.height-touchDev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					//printf("У׼���:\ntouchDev.xfac = %f\n,touchDev.yfac = %f\n,touchDev.xoff = %d\n,touchDev.yoff = %d\n,touchDev.touchtype = %d\n",touchDev.xfac,touchDev.yfac,touchDev.xoff,touchDev.yoff,touchDev.touchtype);
					if(abs(touchDev.xfac)>2||abs(touchDev.yfac)>2)//������Ԥ����෴��.
					{
						cnt=0;
 				    	TouchDrawPoint(lcdDevice.width-20,lcdDevice.height-20,WHITE);	//�����4
   	 					TouchDrawPoint(20,20,RED);								//����1
						LCD_ShowString(40,26,lcdDevice.width,lcdDevice.height,16,"TP Need readjust!");
						touchDev.touchtype=!touchDev.touchtype;//�޸Ĵ�������.
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//����
					LCD_ShowString(35,110,lcdDevice.width,lcdDevice.height,16,"Touch Screen Adjust OK!");//У�����
					HAL_Delay(1000);
					touchSaveAdjData();  
 					LCD_Clear(WHITE);//����   
					return;//У�����				 
			}
		}
		HAL_Delay(10);
		outtime++;
		if(outtime>1000)
		{
			touchGetAdjData();
			break;
	 	} 
 	}
}
