#include "stc15f2k60s2.h"


unsigned char SMG[8],tick_8ms;
unsigned int frequence;

unsigned char code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码



void smg(unsigned char wei,unsigned char duan)
{
P0=0xff;
P2=0xC0;P0=T_COM[wei];P2=0x00;
P2=0xE0;P0=~t_display[duan];P2=0x00;	
}

void Timer0Init(void)		//0秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
    TMOD = 0x04;       //0x04 0x05都可
	TL0 = 0x00;		    //设置定时初始值
	TH0 = 0x00;		    //设置定时初始值
	TF0 = 0;	    	//清除TF0标志
	TR0 = 0;		          
}


void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;
	EA=1;
}

void display_frequence (void)
{
	SMG[0]=15;
		SMG[1]=16;
		SMG[2]=16;
	if(frequence>9999){SMG[3]=frequence/10000%10;}else{SMG[3]=16;}
	if(frequence>999)SMG[4]=frequence/1000%10;else{SMG[4]=16;}
	if(frequence>99)SMG[5]=frequence/100%10;else{SMG[5]=16;}
	if(frequence>9)SMG[6]=frequence/10%10;else{SMG[6]=16;}
	if(frequence>0)SMG[7]=frequence%10;else{SMG[7]=16;}
}

int main (void)
{
P2=0x80;P0=0xff;P2=0x00;
P2=0xa0;P0=0x00;P2=0x00;
Timer0Init();
Timer1Init();		//1毫秒@12.000MHz
while(1)
{
display_frequence();
}	
	
}

void frequence_detect (void)
{
	static unsigned int tick_500ms;
	tick_500ms++;
	if(tick_500ms==500)
	{
		TR0 = 0;
		tick_500ms=0;
		frequence=TH0<<8;
		frequence+=TL0;
		frequence*=2;
		TH0=0;
		TL0=0;
		TR0=1;
	}
}


void server (void) interrupt 3
{
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}
smg(tick_8ms,SMG[tick_8ms]);
frequence_detect ();

}



