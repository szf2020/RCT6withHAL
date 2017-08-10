#include "lcd.h"
#include "font.h"
LCD_DEVICE lcdDevice;
//LCD的画笔颜色和背景色	   
uint16_t POINT_COLOR=0x0000;	//画笔颜色
uint16_t BACK_COLOR=0xFFFF;  //背景色 
/*
funName	:initLCDGPIO
input		:
output	:
describe:初始化lcd相关gpio
remark	:
*/
void initLCDGPIO(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_AFIO_REMAP_SWJ_NOJTAG();//PB3,PB4是初始化时为jtag引脚,需要失能jtag
	/*
	PC6:LCD_RD:从TFTLCD读取数据
	PC7:LCD_WR:向TFTLCD写入数据
	PC8:LCD_RS:命令/数据标志0:读写命令1:读写数据
	PC9:LCD_CS:TFTLCD片选
	PC10:LCD_BL:背光
	*/
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,GPIO_PIN_SET);
	//D0-D15:16位双向数据线 
	GPIO_InitStruct.Pin = GPIO_PIN_All;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_All,GPIO_PIN_SET);
	
}
/*
funName	:lcdRegCmd
input		:regAddress:寄存器编号
output	:
describe:写命令
remark	:
*/
void lcdWriteCmd(uint16_t regAddress){
	LCD_RS_CLR;//0:读写命令 
 	LCD_CS_CLR;//片选
	DATAOUT(regAddress);//要传输的内容
	LCD_WR_CLR;//写指令
	LCD_WR_SET;//取消写指令
 	LCD_CS_SET;//取消片选
}
/*
funName	:lcdRegData
input		:RegData:要写入的值
output	:
describe:写数据
remark	:
*/
void lcdWriteData(uint16_t RegData){
	LCD_RS_SET;//1:读写数据
	LCD_CS_CLR;//片选
	DATAOUT(RegData);//要传输的内容
	LCD_WR_CLR;//写指令
	LCD_WR_SET;//取消写指令
	LCD_CS_SET;//取消片选
}
/*
funName	:lcdReturnData
input		:
output	:uint16_t 数据
describe:返回数据
remark	:
*/
uint16_t lcdReturnData(void){
	uint16_t t;
 	GPIOB->CRL=0X88888888; //PB0-7  上拉输入
	GPIOB->CRH=0X88888888; //PB8-15 上拉输入
	GPIOB->ODR=0X0000;     //全部输出0

	LCD_RS_SET;//1:读写数据
	LCD_CS_CLR;//片选
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD_CLR;//读指令
	t=DATAIN;//数据内容
	LCD_RD_SET;//取消读指令
	LCD_CS_SET; //取消片选

	GPIOB->CRL=0X33333333; //PB0-7  上拉输出
	GPIOB->CRH=0X33333333; //PB8-15 上拉输出
	GPIOB->ODR=0XFFFF;    //全部输出高
	return t;  
}
/*
funName	:lcdReadReg
input		:
output	:uint16_t 数据
describe:读寄存器，返回数据
remark	:
*/
uint16_t lcdReadReg(uint16_t regAddress){
	lcdWriteCmd(regAddress);
	return lcdReturnData();
}
/*
funName	:readLcdDeviceId
input		:
output	:
describe:读设备id
remark	:
*/
void readLcdDeviceId(void){
	lcdWriteCmd(0x01);//复位lcd
	HAL_Delay(50);
	lcdWriteCmd(0XD3);//读取设备id，返回4个数据，最后两个为设备id值
	lcdReturnData();
	lcdReturnData();
	lcdDevice.id = lcdReturnData();
	lcdDevice.id<<=8;
	lcdDevice.id |= lcdReturnData();
	printf("redlcdid:%x\n",lcdDevice.id);
}
/*
funName	:defaultScanDir
input		:dir
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左
output	:
describe:扫描方向设置
remark	:
*/
void defaultScanDir(uint8_t dir){
	uint16_t regval=0;
	uint16_t dirreg=0;
	uint16_t temp;
	if(lcdDevice.dir == 1){
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	}
	switch(dir)
	{
		case L2R_U2D://从左到右,从上到下
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://从下到上,从右到左
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36;
	regval|=0X08;
	lcdWriteCmd(dirreg);
	lcdWriteData(regval);
	if(regval&0X20)
	{
		if(lcdDevice.width<lcdDevice.height)//交换X,Y
		{
			temp=lcdDevice.width;
			lcdDevice.width=lcdDevice.height;
			lcdDevice.height=temp;
		}
	}else  
	{
		if(lcdDevice.width>lcdDevice.height)//交换X,Y
		{
			temp=lcdDevice.width;
			lcdDevice.width=lcdDevice.height;
			lcdDevice.height=temp;
		}
	} 
	lcdWriteCmd(lcdDevice.setXCmd); 
	lcdWriteData(0);
	lcdWriteData(0);
	lcdWriteData((lcdDevice.width-1)>>8);
	lcdWriteData((lcdDevice.width-1)&0XFF);
	lcdWriteCmd(lcdDevice.setYCmd); 
	lcdWriteData(0);
	lcdWriteData(0);
	lcdWriteData((lcdDevice.height-1)>>8);
	lcdWriteData((lcdDevice.height-1)&0XFF);
}
/*
funName	:LCD_SetCursor
input		:Xpos，Ypos
output	:
describe:定位
remark	:
*/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos){
	lcdWriteCmd(lcdDevice.setXCmd); 
	lcdWriteData(Xpos>>8);
	lcdWriteData(Xpos&0XFF); 			 
	lcdWriteCmd(lcdDevice.setYCmd); 
	lcdWriteData(Ypos>>8);
	lcdWriteData(Ypos&0XFF); 
}
/*
funName	:LCD_DrawPoint
input		:x，y
output	:
describe:画点
remark	:
*/
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);//设置光标位置 
	lcdWriteCmd(lcdDevice.gramCmd);//开始写入GRAM
	lcdWriteData(POINT_COLOR);
}
//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(uint8_t i)
{
	while(i--);
}
/*
funName	:LCD_ReadPoint
input		:x，y
output	:
describe:去读某个点的颜色值
remark	:
*/
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
 	uint16_t r,g,b;
	if(x>=lcdDevice.width||y>=lcdDevice.height)return 0;	//超过了范围,直接返回		   
	LCD_SetCursor(x,y);
	lcdWriteCmd(0X2E);//9341/6804/3510/1963 发送读GRAM指令
	GPIOB->CRL=0X88888888; 							//PB0-7  上拉输入
	GPIOB->CRH=0X88888888; 							//PB8-15 上拉输入
	GPIOB->ODR=0XFFFF;     							//全部输出高

	LCD_RS_SET;
	LCD_CS_CLR;	    
	//读取数据(读GRAM时,第一次为假读)	
	LCD_RD_CLR;		   
	opt_delay(2);									//延时		
 	r=DATAIN;  										//实际坐标颜色 
	LCD_RD_SET;
 	//dummy READ
	LCD_RD_CLR;					   
	opt_delay(2);//延时					   
 	r=DATAIN;  	//实际坐标颜色
	LCD_RD_SET; 
	LCD_RD_CLR;					   
	opt_delay(2);//延时			   
	b=DATAIN;//读取蓝色值  	  
	LCD_RD_SET;
	g=r&0XFF;//对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
	g<<=8;
	LCD_CS_SET;
	GPIOB->CRL=0X33333333; 		//PB0-7  上拉输出
	GPIOB->CRH=0X33333333; 		//PB8-15 上拉输出
	GPIOB->ODR=0XFFFF;    		//全部输出高  
	return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
} 
/*
funName	:LCD_DrawLine
input		:
//x1,y1:起点坐标
//x2,y2:终点坐标  
output	:
describe:画线
remark	:
*/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/*
funName	:LCD_DrawRectangle
input		:
//x1,y1:起点坐标
//x2,y2:终点坐标  
output	:
describe:画矩形
remark	:
*/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
/*
funName	:LCD_Draw_Circle
input		:
//(x,y):中心点
//r    :半径
output	:
describe:在指定位置画一个指定大小的圆
remark	:
*/
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  	LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}

/*
funName	:LCD_Fill
input		:
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
output	:
describe:在指定区域内填充指定颜色
remark	:
*/
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{
		LCD_SetCursor(sx,i);      				//设置光标位置 
		lcdWriteCmd(lcdDevice.gramCmd);     			//开始写入GRAM	  
		for(j=0;j<xlen;j++){
			lcdWriteData(color);	//设置光标位置 	  
		}
			  
	}
	
}  
////在指定区域内填充指定颜色块			 
////(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
////color:要填充的颜色
//void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
//{  
//	u16 height,width;
//	u16 i,j;
//	width=ex-sx+1; 			//得到填充的宽度
//	height=ey-sy+1;			//高度
// 	for(i=0;i<height;i++)
//	{
// 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
//		lcdWriteCmd(lcdDevice.gramCmd);     //开始写入GRAM
//		for(j=0;j<width;j++){
//			lcdWriteData(color[i*width+j]);//写入数据 
//		}
//	}	  
//} 

//清屏函数
//color:要清屏的填充色
void LCD_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcdDevice.width;
	totalpoint*=lcdDevice.height; 			//得到总点数
	LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	lcdWriteCmd(lcdDevice.gramCmd);     		//开始写入GRAM	  	  
	for(index=0;index<totalpoint;index++){
		lcdWriteData(color);	
	}
}  
/*
funName	:setLcdDir
input		:dir 0:竖屏1:横屏
output	:
describe:初始化lcd
remark	:
*/
void setLcdDir(uint8_t dir){
	if(dir == 0){
		lcdDevice.dir=0;	//竖屏
		lcdDevice.width=240;
		lcdDevice.height=320;
		lcdDevice.gramCmd=0X2C;
		lcdDevice.setXCmd=0X2A;
		lcdDevice.setYCmd=0X2B; 
	}else{
		lcdDevice.dir=1;	//横屏
		lcdDevice.width=320;
		lcdDevice.height=240;
		lcdDevice.gramCmd=0X2C;
		lcdDevice.setXCmd=0X2A;
		lcdDevice.setYCmd=0X2B;  	
	}
	defaultScanDir(L2R_U2D);
}
/*
funName	:initTFTLCD
input		:
output	:
describe:初始化lcd
remark	:
*/
void initTFTLCD(void){
	initLCDGPIO();
	readLcdDeviceId();
	//功耗控制B
	lcdWriteCmd(0xCF);
	lcdWriteData(0x00);
	lcdWriteData(0xC1);//手册是 0x81 ，原子是 0xC1
	lcdWriteData(0x30);
	//电源序列控制
	lcdWriteCmd(0xED);
	lcdWriteData(0x64);//手册是 0x55 ，原子是 0x64
	lcdWriteData(0x03);//手册是 0x01 ，原子是 0x03
	lcdWriteData(0x12);//手册是 0x23 ，原子是 0x12
	lcdWriteData(0x81);//手册是 0x01 ，原子是 0x81
	//驱动时序控制A
	lcdWriteCmd(0xE8);  
	lcdWriteData(0x85);//手册是 0x84 ，原子是 0x85
	lcdWriteData(0x10);//手册是 0x11 ，原子是 0x10
	lcdWriteData(0x7A);//手册是 0x7A ，原子是 0x7A
	//功耗控制A
	lcdWriteCmd(0xCB);  
	lcdWriteData(0x39);//手册是 0x39 ，原子是 0x39 
	lcdWriteData(0x2C);//手册是 0x2c ，原子是 0x2c 
	lcdWriteData(0x00);//手册是 0x00 ，原子是 0x00 
	lcdWriteData(0x34);//手册是 0x34 ，原子是 0x34 
	lcdWriteData(0x02);//手册是 0x02 ，原子是 0x02 
	//泵比控制
	lcdWriteCmd(0xF7);  
	lcdWriteData(0x20);//手册是 0x10 ，原子是 0x20
	//驱动时序控制B
	lcdWriteCmd(0xEA);  
	lcdWriteData(0x00);//手册是 0x66 ，原子是 0x00 
	lcdWriteData(0x00);//手册是 0x00 ，原子是 0x00
	//功耗控制1
	lcdWriteCmd(0xC0); 
	lcdWriteData(0x1B);//手册是 0x00 ，原子是 0x1B
	//功耗控制2
	lcdWriteCmd(0xC1);
	lcdWriteData(0x01);//手册是 0x10 ，原子是 0x01
	//VCOM控制1
	lcdWriteCmd(0xC5);
	lcdWriteData(0x30);//手册是 0x31 ，原子是 0x30
	lcdWriteData(0x30);//手册是 0x3C ，原子是 0x30
	//VCOM控制2
	lcdWriteCmd(0xC7);
	lcdWriteData(0XB7);//手册是 0x31 ，原子是 0XB7
	//存储器访问控制
	lcdWriteCmd(0x36);
	lcdWriteData(0x48);//手册是 0x00 ，原子是 0x48
	//像素格式设置
	lcdWriteCmd(0x3A);   
	lcdWriteData(0x55);//手册是 0x66 ，原子是 0x55
	//帧速率控制
	lcdWriteCmd(0xB1);   
	lcdWriteData(0x00);//手册是 0x00 ，原子是 0x00   
	lcdWriteData(0x1A);//手册是 0x00 ，原子是 0x1A
	//显示功能控制
	lcdWriteCmd(0xB6);
	lcdWriteData(0x0A);//手册是 0x0A ，原子是 0x0A    
	lcdWriteData(0xA2);//手册是 0x82 ，原子是 0xA2    
	//使能3G，3 伽马控制
	lcdWriteCmd(0xF2);
	lcdWriteData(0x00);//手册是 0x02 ，原子是 0x00 
	//伽马设置
	lcdWriteCmd(0x26);
	lcdWriteData(0x01);//手册是 0x01 ，原子是 0x01 
	//正极gama校准
	lcdWriteCmd(0xE0);
	lcdWriteData(0x0F); 
	lcdWriteData(0x2A); 
	lcdWriteData(0x28); 
	lcdWriteData(0x08); 
	lcdWriteData(0x0E); 
	lcdWriteData(0x08); 
	lcdWriteData(0x54); 
	lcdWriteData(0XA9); 
	lcdWriteData(0x43); 
	lcdWriteData(0x0A); 
	lcdWriteData(0x0F); 
	lcdWriteData(0x00); 
	lcdWriteData(0x00); 
	lcdWriteData(0x00); 
	lcdWriteData(0x00); 
	//负极gama校准
	lcdWriteCmd(0XE1);
	lcdWriteData(0x00); 
	lcdWriteData(0x15); 
	lcdWriteData(0x17); 
	lcdWriteData(0x07); 
	lcdWriteData(0x11); 
	lcdWriteData(0x06); 
	lcdWriteData(0x2B); 
	lcdWriteData(0x56); 
	lcdWriteData(0x3C); 
	lcdWriteData(0x05); 
	lcdWriteData(0x10); 
	lcdWriteData(0x0F); 
	lcdWriteData(0x3F); 
	lcdWriteData(0x3F); 
	lcdWriteData(0x0F);
	//页地址设置(行地址设置)
	lcdWriteCmd(0x2B); 
	lcdWriteData(0x00);
	lcdWriteData(0x00);
	lcdWriteData(0x01);
	lcdWriteData(0x3f);
	//列地址设置
	lcdWriteCmd(0x2A); 
	lcdWriteData(0x00);
	lcdWriteData(0x00);
	lcdWriteData(0x00);
	lcdWriteData(0xef);	
	//退出睡眠模式
	lcdWriteCmd(0x11); //Exit Sleep
	HAL_Delay(120);
	//开显示
	lcdWriteCmd(0x29);
	setLcdDir(0);//设置竖屏
	LCD_BL_SET;//开背景灯
	LCD_Clear(GREEN);//清屏
}
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{	   
	if(lcdDevice.id==0X9341||lcdDevice.id==0X5310)
	{
		lcdWriteCmd(lcdDevice.setXCmd); 
		lcdWriteData(x>>8);
		lcdWriteData(x&0XFF);  			 
		lcdWriteCmd(lcdDevice.setYCmd); 
		lcdWriteData(y>>8);
		lcdWriteData(y&0XFF); 		 	 
	}
	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(lcdDevice.gramCmd);//写指令  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	lcdWriteData(color);		//写数据
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{  							  
  uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcdDevice.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcdDevice.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}  
//m^n函数
//返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~4294967295);	 
//len:长度(即要显示的位数)如果len没有到达num的位数，取num后len位
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{  
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
////设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
////sx,sy:窗口起始坐标(左上角)
////width,height:窗口宽度和高度,必须大于0!!
////窗体大小:width*height. 
//void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
//{
//	uint16_t twidth,theight;
//	twidth=sx+width-1;
//	theight=sy+height-1;
//	lcdWriteCmd(lcdDevice.setXCmd); 
//	lcdWriteData(sx>>8); 
//	lcdWriteData(sx&0XFF);	 
//	lcdWriteData(twidth>>8); 
//	lcdWriteData(twidth&0XFF);  
//	lcdWriteCmd(lcdDevice.setYCmd); 
//	lcdWriteData(sy>>8); 
//	lcdWriteData(sy&0XFF); 
//	lcdWriteData(theight>>8); 
//	lcdWriteData(theight&0XFF); 
//}
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{         
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
