#include<STC15F2K60S2.H>
#include<INTRINS.H>
#include<IIC.H>
unsigned char code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码
unsigned char tick_8ms,smg[8],content;

void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;    //开启定时器1
	EA  = 1;    //开启总中断
}

void SMG (unsigned char wei,unsigned char duan)
{
P2=0XC0;P0=T_COM[wei];P2=0X00;
P2=0XE0;P0=~t_display[duan];P2=0X00;
}


void Delay4ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 47;
	j = 174;
	do
	{
		while (--j);
	} while (--i);
}





int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//关闭led
P2=0XA0;P0=0X00;P2=0X00;//关闭蜂鸣器关闭继电器
//write24c02(0x01,0x03);
Delay4ms();
content=read24c02(0x01);	
Timer1Init();
while(1)
{


smg[0]=content;
	
	
	
}
}

void server (void) interrupt 3
{
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}
SMG(tick_8ms,smg[tick_8ms]);
	
}
