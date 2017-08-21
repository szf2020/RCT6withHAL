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
describe:向触摸屏IC写入1byte数据
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
		TP_SCK_HIGH();		//上升沿有效	        
	}		 			
}
/*
funName	:touchReadADC
input		:cmd
output	:读到的数据	 
describe:从触摸屏IC读取adc值
remark	:
*/
uint16_t touchReadADC(uint8_t cmd){
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TP_SCK_LOW();		//先拉低时钟 	 
	TP_MOSI_LOW();	//拉低数据线
	TP_CS_LOW(); 		//选中触摸屏IC
	touchWriteByte(cmd);//发送命令字
	HAL_Delay(1);//ADS7846的转换时间最长为6us
	TP_SCK_LOW(); 	     	    
	HAL_Delay(1);
	TP_SCK_HIGH();		//给1个时钟，清除BUSY	    	    
	TP_SCK_LOW();	     	    
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		TP_SCK_LOW();	//下降沿有效  	    	   
		TP_SCK_HIGH();
		if(IS_TP_MISO_HIGH)Num++; 		 
	}  	
	Num>>=4;   	//只有高12位有效.
	TP_CS_HIGH();//释放片选	
	//printf("cmd = %d,Num = %d\n",cmd,Num);
	return(Num);   
}
/*
funName	:touchReadXOrY
input		:xy
output	:读到的数据	 
describe:读取一个坐标值(x或者y)
remark	:连续读取READ_TIMES次数据,对这些数据升序排列,然后去掉最低和最高LOST_VAL个数,取平均值 
*/
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
uint16_t touchReadXOrY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=touchReadADC(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
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
input		:x,y读取到的坐标值
output	:0失败1成功
describe:读取x,y坐标
remark	:最小值不能少于100.
*/

uint8_t touchReadXY(uint16_t *x,uint16_t *y)
{
	//默认为touchtype=0的数据.
	uint8_t cmdRdx = 0xd0;
	uint8_t cmdRdy = 0x90;
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=touchReadXOrY(cmdRdx);
	ytemp=touchReadXOrY(cmdRdy);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	//printf("xtemp = %d,ytemp = %d\n",xtemp,ytemp);
	return 1;//读数成功
}
/*
funName	:touchReadXY2
input		:x,y读取到的坐标值
output	:0失败1成功
describe:连续2次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	 
remark	:该函数能大大提高准确度
*/
#define ERR_RANGE 50 //误差范围 
uint8_t touchReadXY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=touchReadXY(&x1,&y1);   
    if(flag==0)return(0);
    flag=touchReadXY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
			//printf("xtemp = %d,ytemp = %d\n",*x,*y);
        return 1;
    }else return 0;	  
}  
/*----------------------------------与LCD有关部分------------------------------------------------------*/
/*
funName	:TouchDrawPoint
input		:x,y,color
output	:
describe:画一个触摸点
remark	:
*/
void TouchDrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	POINT_COLOR = color;
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//画中心圈
}
/*
funName	:touchDrawBigPoint
input		:x,y,color
output	:
describe:画一个大点
remark	:
*/
void touchDrawBigPoint(uint16_t x,uint16_t y,uint16_t color){
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//中心点 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);
}
/*
funName	:touchTest
input		:
output	:
describe:判断屏幕是否校准，没校准则校准
remark	:
*/
void touchTest(void){
	if(touchGetAdjData()){
//		printf("已经校准!\n");
	}//已经校准
	else			   //未校准?
	{ 										    
		LCD_Clear(WHITE);//清屏
		adjustTouch();  //屏幕校准 
		touchSaveAdjData();	 
	}			
	touchGetAdjData();
}
/*
funName	:initTouch
input		:
output	:
describe:初始化触摸屏
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
	
	touchReadXY(&touchDev.x[0],&touchDev.y[0]);//第一次读取初始化	 
	LCD_Clear(YELLOW);//清屏
	touchTest();
}
/*
funName	:scanTouch
input		:tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
output	:触摸状态：按下1/松开0
describe:触摸扫描
remark	:
*/
uint8_t scanTouch(uint8_t tp){
	//printf("%d",IS_TP_PEN_LOW);
	if(IS_TP_PEN_LOW)//有按键按下
	{
		if(tp)touchReadXY2(&touchDev.x[0],&touchDev.y[0]);//读取物理坐标
		else if(touchReadXY2(&touchDev.x[0],&touchDev.y[0]))//读取屏幕坐标
		{
	 		touchDev.x[0]=touchDev.xfac*touchDev.x[0]+touchDev.xoff;//将结果转换为屏幕坐标
			touchDev.y[0]=touchDev.yfac*touchDev.y[0]+touchDev.yoff;  
//			printf("x = %d,y = %d\n",touchDev.x[0],touchDev.y[0]);
	 	} 
		if((touchDev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			touchDev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			touchDev.x[4]=touchDev.x[0];//记录第一次按下时的坐标
			touchDev.y[4]=touchDev.y[0];  	   			 
		}
		
	}else
	{
		if(touchDev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			touchDev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			touchDev.x[4]=0;
			touchDev.y[4]=0;
			touchDev.x[0]=0xffff;
			touchDev.y[0]=0xffff;
		}	    
	}
	touchDrawBigPoint(touchDev.x[0],touchDev.y[0],RED);
	return touchDev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}
/*
funName	:touchSaveAdjData
input		:
output	:
describe:保存校准数据到eeprom
remark	:
*/
//保存在EEPROM里面的地址区间基址,占用13个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
#define SAVE_ADDR_BASE 40
void touchSaveAdjData(void){
	uint32_t temp;			
	uint8_t data[4];	
	//保存校正结果!		   							  
	temp=touchDev.xfac*100000000;//保存x校正因素
	data[0] =(uint8_t)temp & 0xFF;
	data[1] =(uint8_t)((temp >> 8) & 0xFF);
	data[2] =(uint8_t)((temp >> 16) & 0xFF);
	data[3] =(uint8_t)((temp >> 24) & 0xFF);
  eepromWriteBytes(data,SAVE_ADDR_BASE,4);  
	//printf("temp = %d,xfac:%d %d %d %d\n",temp,data[0],data[1],data[2],data[3]);	
	temp=touchDev.yfac*100000000;//保存y校正因素  
	data[0] =(uint8_t)temp & 0xFF;
	data[1] =(uint8_t)((temp >> 8) & 0xFF);
	data[2] =(uint8_t)((temp >> 16) & 0xFF);
	data[3] =(uint8_t)((temp >> 24) & 0xFF);  
  eepromWriteBytes(data,SAVE_ADDR_BASE+4,4);
	//printf("temp = %d,yfac:%d %d %d %d\n",temp,data[0],data[1],data[2],data[3]);
	//保存x偏移量
	data[0] =(uint8_t)touchDev.xoff & 0xFF;
	data[1] =(uint8_t)((touchDev.xoff >> 8) & 0xFF);
   eepromWriteBytes(data,SAVE_ADDR_BASE+8,2);	
	// printf("touchDev.xoff = %d,xoff:%d %d\n",touchDev.xoff,data[0],data[1]);	 
	//保存y偏移量
	data[0] =(uint8_t)touchDev.yoff & 0xFF;
	data[1] =(uint8_t)((touchDev.yoff >> 8) & 0xFF);
	eepromWriteBytes(data,SAVE_ADDR_BASE+10,2);	
	//printf("touchDev.yoff = %d,yoff:%d %d\n",touchDev.yoff,data[0],data[1]);
	//保存触屏类型
	eepromWriteBytes(&(touchDev.touchtype),SAVE_ADDR_BASE+12,1);	
	temp=0X0A;//标记校准过了
	data[0] =(uint8_t)temp & 0xFF;
	eepromWriteBytes(data,SAVE_ADDR_BASE+13,1);
}
/*
funName	:touchGetAdjData
input		:
output	:0没有校准数据1获取成功
describe:获取校准数据
remark	:
*/
uint8_t touchGetAdjData(void){
	uint8_t data[4];
	uint32_t temp;
	eepromReadBytes(data,SAVE_ADDR_BASE+13,1);
	if(data[0] == 0X0A){
		eepromReadBytes(data,SAVE_ADDR_BASE,4);
		temp = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 16);
		touchDev.xfac=(float)temp/100000000;//得到x校准参数
		eepromReadBytes(data,SAVE_ADDR_BASE+4,4);
		temp = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 16);
		touchDev.yfac=(float)temp/100000000;//得到y校准参数
		//得到x偏移量
		eepromReadBytes(data,SAVE_ADDR_BASE + 8,2);
		temp = data[0] | (data[1] << 8);
		touchDev.xoff = temp;			   	  
 	    //得到y偏移量
		eepromReadBytes(data,SAVE_ADDR_BASE + 10,2);
		temp = data[0] | (data[1] << 8);
		touchDev.yoff = temp;			
		
		eepromReadBytes(data,SAVE_ADDR_BASE + 12,1);
 		touchDev.touchtype=data[0];//读取触屏类型标记
		//printf("touchDev.xfac = %f\n,touchDev.yfac = %f\n,touchDev.xoff = %d\n,touchDev.yoff = %d\n,touchDev.touchtype = %d\n",touchDev.xfac,touchDev.yfac,touchDev.xoff,touchDev.yoff,touchDev.touchtype);
		return 1;
	}
	return 0;
}
//提示字符串
const u8* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
//提示校准结果(各个参数)
/*
funName	:touchAdjInfoShow
input		:
output	:
describe:校验提示信息
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
	LCD_ShowNum(40+24,160,x0,4,16);		//显示数值
	LCD_ShowNum(40+24+80,160,y0,4,16);	//显示数值
	LCD_ShowNum(40+24,180,x1,4,16);		//显示数值
	LCD_ShowNum(40+24+80,180,y1,4,16);	//显示数值
	LCD_ShowNum(40+24,200,x2,4,16);		//显示数值
	LCD_ShowNum(40+24+80,200,y2,4,16);	//显示数值
	LCD_ShowNum(40+24,220,x3,4,16);		//显示数值
	LCD_ShowNum(40+24+80,220,y3,4,16);	//显示数值
 	LCD_ShowNum(40+56,lcdDevice.width,fac,3,16); 	//显示数值,该数值必须在95~105范围之内.

}
//校准点参数：(0,1)与(2,3)，(0,2)与(1,3),(1,2)与(0,3)，这三组点的距离
const u8 TP_ADJDIS_TBL[3][4]={{0,1,2,3},{0,2,1,3},{1,2,0,3}};//校准距离计算表
//触摸屏校准代码
//得到四个校准参数
/*
funName	:adjustTouch
input		:
output	:
describe:校验触摸屏
remark	:
*/
void adjustTouch(void){
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	
	u16 outtime=0; 	  
	LCD_Clear(WHITE);	//清屏 	    
	POINT_COLOR=BLUE;	//蓝色
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//显示提示信息
	TouchDrawPoint(20,20,RED);//画点1 
	touchDev.sta=0;//消除触发信号 
	touchDev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
READJ:
		touchDev.scan(1);//扫描物理坐标
		if((touchDev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			touchDev.sta&=~(1<<6);	//标记按键已经被处理过了. 
			pos_temp[cnt][0]=touchDev.x[0];
			pos_temp[cnt][1]=touchDev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TouchDrawPoint(20,20,WHITE);				//清除点1 
					TouchDrawPoint(lcdDevice.width-20,20,RED);	//画点2
					break;
				case 2:
 					TouchDrawPoint(lcdDevice.width-20,20,WHITE);	//清除点2
					TouchDrawPoint(20,lcdDevice.height-20,RED);	//画点3
					break;
				case 3:
 					TouchDrawPoint(20,lcdDevice.height-20,WHITE);			//清除点3
 					TouchDrawPoint(lcdDevice.width-20,lcdDevice.height-20,RED);	//画点4
					break;                                                                                                                                                                                                                                                          
				case 4:	 //全部四个点已经得到
					for(cnt=0;cnt<3;cnt++)//计算三组点的距离是否在允许范围内？
					{ 
						tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][0]-pos_temp[TP_ADJDIS_TBL[cnt][1]][0]);//x1-x2/x1-x3/x2-x3
						tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][0]][1]-pos_temp[TP_ADJDIS_TBL[cnt][1]][1]);//y1-y2/y1-y3/y2-y3
						tem1*=tem1;
						tem2*=tem2;
						d1=sqrt(tem1+tem2);//得到两点之间的距离 
						tem1=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][0]-pos_temp[TP_ADJDIS_TBL[cnt][3]][0]);//x3-x4/x2-x4/x1-x4
						tem2=abs(pos_temp[TP_ADJDIS_TBL[cnt][2]][1]-pos_temp[TP_ADJDIS_TBL[cnt][3]][1]);//y3-y4/y2-y4/y1-y4
						tem1*=tem1;
						tem2*=tem2;
						d2=sqrt(tem1+tem2);//得到两点之间的距离
						fac=(float)d1/d2;
						if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
						{
							cnt=0;
							TouchDrawPoint(lcdDevice.width-20,lcdDevice.height-20,WHITE);	//清除点4
							TouchDrawPoint(20,20,RED);									//画点1
							touchAdjInfoShow(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
							goto READJ;	//不合格，重新校准
						}
					}  
					//正确了
					//计算结果
					touchDev.xfac=(float)(lcdDevice.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					touchDev.xoff=(lcdDevice.width-touchDev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
 					touchDev.yfac=(float)(lcdDevice.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					touchDev.yoff=(lcdDevice.height-touchDev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					//printf("校准结果:\ntouchDev.xfac = %f\n,touchDev.yfac = %f\n,touchDev.xoff = %d\n,touchDev.yoff = %d\n,touchDev.touchtype = %d\n",touchDev.xfac,touchDev.yfac,touchDev.xoff,touchDev.yoff,touchDev.touchtype);
					if(abs(touchDev.xfac)>2||abs(touchDev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
 				    	TouchDrawPoint(lcdDevice.width-20,lcdDevice.height-20,WHITE);	//清除点4
   	 					TouchDrawPoint(20,20,RED);								//画点1
						LCD_ShowString(40,26,lcdDevice.width,lcdDevice.height,16,"TP Need readjust!");
						touchDev.touchtype=!touchDev.touchtype;//修改触屏类型.
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//清屏
					LCD_ShowString(35,110,lcdDevice.width,lcdDevice.height,16,"Touch Screen Adjust OK!");//校正完成
					HAL_Delay(1000);
					touchSaveAdjData();  
 					LCD_Clear(WHITE);//清屏   
					return;//校正完成				 
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
