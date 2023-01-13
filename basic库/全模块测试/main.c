#include "stc15f2k60s2.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
#include "intrins.h"
#include "stdio.h"

#define TX P10
#define RX P11

unsigned char code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

bit LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8,BUZZER,RELAY;

unsigned char SMG[8],key_value,schedule_tick;
int tempreture;

void BASIC (void)
{
P0=0xFF;P2=0x80;
P00=~LED1;P01=~LED2;P02=~LED3;P03=~LED4;P04=~LED5;P05=~LED6;P06=~LED7;P07=~LED8;
P2=0x00;
	
P0=0x00;P2=0xA0;
P04=RELAY;P06=BUZZER;
P2=0x00;	
}

void smg (unsigned char wei,unsigned char duan)
{
P0=0xff;
P2=0xc0;P0=T_COM[wei];P2=0x00;
P2=0xe0;P0=~t_display[duan];P2=0x00;
}

void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04; 
	EA=1;
}


void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xC7;		//设定定时初值
	TH1 = 0xFE;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}

char putchar (char c)
{
SBUF=c;
while(!TI);
TI=0;
return c;
}

void config_ds1302 (unsigned char shi,unsigned char fen,unsigned char miao)
{
Write_Ds1302_Byte(0x8e,0x00);
Write_Ds1302_Byte(0x80,miao);
Write_Ds1302_Byte(0x82,fen);
Write_Ds1302_Byte(0x84,shi);	
Write_Ds1302_Byte(0x8e,0x80);
}

void read_ds1302 (void)
{
SMG[0]=Read_Ds1302_Byte(0X85)/16;
SMG[1]=Read_Ds1302_Byte(0X85)%16;
SMG[2]=17;
SMG[3]=Read_Ds1302_Byte(0X83)/16;
SMG[4]=Read_Ds1302_Byte(0X83)%16;
SMG[5]=17;
SMG[6]=Read_Ds1302_Byte(0X81)/16;
SMG[7]=Read_Ds1302_Byte(0X81)%16;	
}

void display_temp (void)
{
tempreture=(int)(_read_ds18b20()*100);	
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=tempreture/1000%10;
SMG[5]=(tempreture/100%10)+32;
SMG[6]=tempreture/10%10;
SMG[7]=tempreture%10;	
}




void display_voltage (unsigned char addr)
{
unsigned int vol;
vol=(((float)adc(addr))/255)*500;
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=vol/1000%10;
SMG[5]=(vol/100%10)+32;
SMG[6]=vol/10%10;
SMG[7]=vol%10;
}

void display_24c02 (unsigned char addr)
{
unsigned char memory;
memory=read_24c02 (addr);
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=17;
SMG[5]=memory/100%10;
SMG[6]=memory/10%10;
SMG[7]=memory%10;
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

void Timer0Init(void)		//12微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

unsigned int ceju (void)
{
unsigned char times=10;
unsigned int juli;
Timer0Init();	
	TX=0;
while(times--)
{
while(!TF0);
TX=1;
TF0=0;
}
TR0=0;TH0=0;TL0=0;TR0=1;
while(RX&&!TF0);
if(TF0==1){juli=999;}
else{juli=((TH0<<8)+TL0)*0.017;}
TR0=0;
return juli;
}


void display_distance (void)
{
unsigned int distance;
distance=ceju();
printf("距离为%d\r\n",distance);
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=17;
SMG[5]=distance/100%10;
SMG[6]=distance/10%10;
SMG[7]=distance%10;
}

void read_key (void)
{
char trig=0;
unsigned char key_val;	
trig=0;	
P44=0;P42=1;P35=1;P34=1;
if(P30==0){while(!P30){}trig=1;key_val=1;}
if(P31==0){while(!P31){}trig=1;key_val=2;}	
if(P32==0){while(!P32){}trig=1;key_val=3;}	
if(P33==0){while(!P33){}trig=1;key_val=4;}
P44=1;P42=0;P35=1;P34=1;
if(P30==0){while(!P30){}trig=1;key_val=5;}
if(P31==0){while(!P31){}trig=1;key_val=6;}	
if(P32==0){while(!P32){}trig=1;key_val=7;}	
if(P33==0){while(!P33){}trig=1;key_val=8;}
P44=1;P42=1;P35=0;P34=1;
if(P30==0){while(!P30){}trig=1;key_val=9;}
if(P31==0){while(!P31){}trig=1;key_val=10;}	
if(P32==0){while(!P32){}trig=1;key_val=11;}	
if(P33==0){while(!P33){}trig=1;key_val=12;}
P44=1;P42=1;P35=1;P34=0;
if(P30==0){while(!P30){}trig=1;key_val=13;}
if(P31==0){while(!P31){}trig=1;key_val=14;}	
if(P32==0){while(!P32){}trig=1;key_val=15;}	
if(P33==0){while(!P33){}trig=1;key_val=16;}

if(!trig){key_val=99;}
key_value=key_val;
}

void schedule (void)
{
if(schedule_tick==30)//10ms执行一次
{
schedule_tick=0;
//	read_ds1302 ();	
//display_24c02(0x01);
display_temp ();	
//display_distance ();
//display_voltage (0x03);
	
	
}

}





int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;
P2=0XA0;P0=0X00;P2=0X00;
config_ds1302(0x23,0x59,0x58);
Timer2Init();
UartInit();
//write_24c02(0x01,0x80);
//Delay4ms();	
while(1)
{
read_key ();
schedule ();

}	
}





void server (void) interrupt 12
{
static unsigned char tick_8ms;
schedule_tick++;
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}
smg(tick_8ms,SMG[tick_8ms]);
BASIC ();
}