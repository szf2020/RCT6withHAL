#include "lcd.h"
#include "font.h"
LCD_DEVICE lcdDevice;
//LCD�Ļ�����ɫ�ͱ���ɫ	   
uint16_t POINT_COLOR=0x0000;	//������ɫ
uint16_t BACK_COLOR=0xFFFF;  //����ɫ 
/*
funName	:initLCDGPIO
input		:
output	:
describe:��ʼ��lcd���gpio
remark	:
*/
void initLCDGPIO(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_AFIO_REMAP_SWJ_NOJTAG();//PB3,PB4�ǳ�ʼ��ʱΪjtag����,��Ҫʧ��jtag
	/*
	PC6:LCD_RD:��TFTLCD��ȡ����
	PC7:LCD_WR:��TFTLCDд������
	PC8:LCD_RS:����/���ݱ�־0:��д����1:��д����
	PC9:LCD_CS:TFTLCDƬѡ
	PC10:LCD_BL:����
	*/
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,GPIO_PIN_SET);
	//D0-D15:16λ˫�������� 
	GPIO_InitStruct.Pin = GPIO_PIN_All;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_All,GPIO_PIN_SET);
	
}
/*
funName	:lcdRegCmd
input		:regAddress:�Ĵ������
output	:
describe:д����
remark	:
*/
void lcdWriteCmd(uint16_t regAddress){
	LCD_RS_CLR;//0:��д���� 
 	LCD_CS_CLR;//Ƭѡ
	DATAOUT(regAddress);//Ҫ���������
	LCD_WR_CLR;//дָ��
	LCD_WR_SET;//ȡ��дָ��
 	LCD_CS_SET;//ȡ��Ƭѡ
}
/*
funName	:lcdRegData
input		:RegData:Ҫд���ֵ
output	:
describe:д����
remark	:
*/
void lcdWriteData(uint16_t RegData){
	LCD_RS_SET;//1:��д����
	LCD_CS_CLR;//Ƭѡ
	DATAOUT(RegData);//Ҫ���������
	LCD_WR_CLR;//дָ��
	LCD_WR_SET;//ȡ��дָ��
	LCD_CS_SET;//ȡ��Ƭѡ
}
/*
funName	:lcdReturnData
input		:
output	:uint16_t ����
describe:��������
remark	:
*/
uint16_t lcdReturnData(void){
	uint16_t t;
 	GPIOB->CRL=0X88888888; //PB0-7  ��������
	GPIOB->CRH=0X88888888; //PB8-15 ��������
	GPIOB->ODR=0X0000;     //ȫ�����0

	LCD_RS_SET;//1:��д����
	LCD_CS_CLR;//Ƭѡ
	//��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;//��ָ��
	t=DATAIN;//��������
	LCD_RD_SET;//ȡ����ָ��
	LCD_CS_SET; //ȡ��Ƭѡ

	GPIOB->CRL=0X33333333; //PB0-7  �������
	GPIOB->CRH=0X33333333; //PB8-15 �������
	GPIOB->ODR=0XFFFF;    //ȫ�������
	return t;  
}
/*
funName	:lcdReadReg
input		:
output	:uint16_t ����
describe:���Ĵ�������������
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
describe:���豸id
remark	:
*/
void readLcdDeviceId(void){
	lcdWriteCmd(0x01);//��λlcd
	HAL_Delay(50);
	lcdWriteCmd(0XD3);//��ȡ�豸id������4�����ݣ��������Ϊ�豸idֵ
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
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���
output	:
describe:ɨ�跽������
remark	:
*/
void defaultScanDir(uint8_t dir){
	uint16_t regval=0;
	uint16_t dirreg=0;
	uint16_t temp;
	if(lcdDevice.dir == 1){
		switch(dir)//����ת��
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
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36;
	regval|=0X08;
	lcdWriteCmd(dirreg);
	lcdWriteData(regval);
	if(regval&0X20)
	{
		if(lcdDevice.width<lcdDevice.height)//����X,Y
		{
			temp=lcdDevice.width;
			lcdDevice.width=lcdDevice.height;
			lcdDevice.height=temp;
		}
	}else  
	{
		if(lcdDevice.width>lcdDevice.height)//����X,Y
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
input		:Xpos��Ypos
output	:
describe:��λ
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
input		:x��y
output	:
describe:����
remark	:
*/
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	lcdWriteCmd(lcdDevice.gramCmd);//��ʼд��GRAM
	lcdWriteData(POINT_COLOR);
}
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(uint8_t i)
{
	while(i--);
}
/*
funName	:LCD_ReadPoint
input		:x��y
output	:
describe:ȥ��ĳ�������ɫֵ
remark	:
*/
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
 	uint16_t r,g,b;
	if(x>=lcdDevice.width||y>=lcdDevice.height)return 0;	//�����˷�Χ,ֱ�ӷ���		   
	LCD_SetCursor(x,y);
	lcdWriteCmd(0X2E);//9341/6804/3510/1963 ���Ͷ�GRAMָ��
	GPIOB->CRL=0X88888888; 							//PB0-7  ��������
	GPIOB->CRH=0X88888888; 							//PB8-15 ��������
	GPIOB->ODR=0XFFFF;     							//ȫ�������

	LCD_RS_SET;
	LCD_CS_CLR;	    
	//��ȡ����(��GRAMʱ,��һ��Ϊ�ٶ�)	
	LCD_RD_CLR;		   
	opt_delay(2);									//��ʱ		
 	r=DATAIN;  										//ʵ��������ɫ 
	LCD_RD_SET;
 	//dummy READ
	LCD_RD_CLR;					   
	opt_delay(2);//��ʱ					   
 	r=DATAIN;  	//ʵ��������ɫ
	LCD_RD_SET; 
	LCD_RD_CLR;					   
	opt_delay(2);//��ʱ			   
	b=DATAIN;//��ȡ��ɫֵ  	  
	LCD_RD_SET;
	g=r&0XFF;//����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
	g<<=8;
	LCD_CS_SET;
	GPIOB->CRL=0X33333333; 		//PB0-7  �������
	GPIOB->CRH=0X33333333; 		//PB8-15 �������
	GPIOB->ODR=0XFFFF;    		//ȫ�������  
	return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
} 
/*
funName	:LCD_DrawLine
input		:
//x1,y1:�������
//x2,y2:�յ�����  
output	:
describe:����
remark	:
*/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
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
//x1,y1:�������
//x2,y2:�յ�����  
output	:
describe:������
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
//(x,y):���ĵ�
//r    :�뾶
output	:
describe:��ָ��λ�û�һ��ָ����С��Բ
remark	:
*/
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
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
//�����С:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:Ҫ������ɫ
output	:
describe:��ָ�����������ָ����ɫ
remark	:
*/
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{
		LCD_SetCursor(sx,i);      				//���ù��λ�� 
		lcdWriteCmd(lcdDevice.gramCmd);     			//��ʼд��GRAM	  
		for(j=0;j<xlen;j++){
			lcdWriteData(color);	//���ù��λ�� 	  
		}
			  
	}
	
}  
////��ָ�����������ָ����ɫ��			 
////(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
////color:Ҫ������ɫ
//void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
//{  
//	u16 height,width;
//	u16 i,j;
//	width=ex-sx+1; 			//�õ����Ŀ��
//	height=ey-sy+1;			//�߶�
// 	for(i=0;i<height;i++)
//	{
// 		LCD_SetCursor(sx,sy+i);   	//���ù��λ�� 
//		lcdWriteCmd(lcdDevice.gramCmd);     //��ʼд��GRAM
//		for(j=0;j<width;j++){
//			lcdWriteData(color[i*width+j]);//д������ 
//		}
//	}	  
//} 

//��������
//color:Ҫ���������ɫ
void LCD_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcdDevice.width;
	totalpoint*=lcdDevice.height; 			//�õ��ܵ���
	LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
	lcdWriteCmd(lcdDevice.gramCmd);     		//��ʼд��GRAM	  	  
	for(index=0;index<totalpoint;index++){
		lcdWriteData(color);	
	}
}  
/*
funName	:setLcdDir
input		:dir 0:����1:����
output	:
describe:��ʼ��lcd
remark	:
*/
void setLcdDir(uint8_t dir){
	if(dir == 0){
		lcdDevice.dir=0;	//����
		lcdDevice.width=240;
		lcdDevice.height=320;
		lcdDevice.gramCmd=0X2C;
		lcdDevice.setXCmd=0X2A;
		lcdDevice.setYCmd=0X2B; 
	}else{
		lcdDevice.dir=1;	//����
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
describe:��ʼ��lcd
remark	:
*/
void initTFTLCD(void){
	initLCDGPIO();
	readLcdDeviceId();
	//���Ŀ���B
	lcdWriteCmd(0xCF);
	lcdWriteData(0x00);
	lcdWriteData(0xC1);//�ֲ��� 0x81 ��ԭ���� 0xC1
	lcdWriteData(0x30);
	//��Դ���п���
	lcdWriteCmd(0xED);
	lcdWriteData(0x64);//�ֲ��� 0x55 ��ԭ���� 0x64
	lcdWriteData(0x03);//�ֲ��� 0x01 ��ԭ���� 0x03
	lcdWriteData(0x12);//�ֲ��� 0x23 ��ԭ���� 0x12
	lcdWriteData(0x81);//�ֲ��� 0x01 ��ԭ���� 0x81
	//����ʱ�����A
	lcdWriteCmd(0xE8);  
	lcdWriteData(0x85);//�ֲ��� 0x84 ��ԭ���� 0x85
	lcdWriteData(0x10);//�ֲ��� 0x11 ��ԭ���� 0x10
	lcdWriteData(0x7A);//�ֲ��� 0x7A ��ԭ���� 0x7A
	//���Ŀ���A
	lcdWriteCmd(0xCB);  
	lcdWriteData(0x39);//�ֲ��� 0x39 ��ԭ���� 0x39 
	lcdWriteData(0x2C);//�ֲ��� 0x2c ��ԭ���� 0x2c 
	lcdWriteData(0x00);//�ֲ��� 0x00 ��ԭ���� 0x00 
	lcdWriteData(0x34);//�ֲ��� 0x34 ��ԭ���� 0x34 
	lcdWriteData(0x02);//�ֲ��� 0x02 ��ԭ���� 0x02 
	//�ñȿ���
	lcdWriteCmd(0xF7);  
	lcdWriteData(0x20);//�ֲ��� 0x10 ��ԭ���� 0x20
	//����ʱ�����B
	lcdWriteCmd(0xEA);  
	lcdWriteData(0x00);//�ֲ��� 0x66 ��ԭ���� 0x00 
	lcdWriteData(0x00);//�ֲ��� 0x00 ��ԭ���� 0x00
	//���Ŀ���1
	lcdWriteCmd(0xC0); 
	lcdWriteData(0x1B);//�ֲ��� 0x00 ��ԭ���� 0x1B
	//���Ŀ���2
	lcdWriteCmd(0xC1);
	lcdWriteData(0x01);//�ֲ��� 0x10 ��ԭ���� 0x01
	//VCOM����1
	lcdWriteCmd(0xC5);
	lcdWriteData(0x30);//�ֲ��� 0x31 ��ԭ���� 0x30
	lcdWriteData(0x30);//�ֲ��� 0x3C ��ԭ���� 0x30
	//VCOM����2
	lcdWriteCmd(0xC7);
	lcdWriteData(0XB7);//�ֲ��� 0x31 ��ԭ���� 0XB7
	//�洢�����ʿ���
	lcdWriteCmd(0x36);
	lcdWriteData(0x48);//�ֲ��� 0x00 ��ԭ���� 0x48
	//���ظ�ʽ����
	lcdWriteCmd(0x3A);   
	lcdWriteData(0x55);//�ֲ��� 0x66 ��ԭ���� 0x55
	//֡���ʿ���
	lcdWriteCmd(0xB1);   
	lcdWriteData(0x00);//�ֲ��� 0x00 ��ԭ���� 0x00   
	lcdWriteData(0x1A);//�ֲ��� 0x00 ��ԭ���� 0x1A
	//��ʾ���ܿ���
	lcdWriteCmd(0xB6);
	lcdWriteData(0x0A);//�ֲ��� 0x0A ��ԭ���� 0x0A    
	lcdWriteData(0xA2);//�ֲ��� 0x82 ��ԭ���� 0xA2    
	//ʹ��3G��3 ٤�����
	lcdWriteCmd(0xF2);
	lcdWriteData(0x00);//�ֲ��� 0x02 ��ԭ���� 0x00 
	//٤������
	lcdWriteCmd(0x26);
	lcdWriteData(0x01);//�ֲ��� 0x01 ��ԭ���� 0x01 
	//����gamaУ׼
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
	//����gamaУ׼
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
	//ҳ��ַ����(�е�ַ����)
	lcdWriteCmd(0x2B); 
	lcdWriteData(0x00);
	lcdWriteData(0x00);
	lcdWriteData(0x01);
	lcdWriteData(0x3f);
	//�е�ַ����
	lcdWriteCmd(0x2A); 
	lcdWriteData(0x00);
	lcdWriteData(0x00);
	lcdWriteData(0x00);
	lcdWriteData(0xef);	
	//�˳�˯��ģʽ
	lcdWriteCmd(0x11); //Exit Sleep
	HAL_Delay(120);
	//����ʾ
	lcdWriteCmd(0x29);
	setLcdDir(0);//��������
	LCD_BL_SET;//��������
	LCD_Clear(GREEN);//����
}
//���ٻ���
//x,y:����
//color:��ɫ
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
	DATAOUT(lcdDevice.gramCmd);//дָ��  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	lcdWriteData(color);		//д����
}

//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{  							  
  uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcdDevice.height)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcdDevice.width)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}  
//m^n����
//����ֵ:m^n�η�.
uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
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
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~4294967295);	 
//len:����(��Ҫ��ʾ��λ��)���lenû�е���num��λ����ȡnum��lenλ
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
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
////���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
////sx,sy:������ʼ����(���Ͻ�)
////width,height:���ڿ�Ⱥ͸߶�,�������0!!
////�����С:width*height. 
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
//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{         
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
