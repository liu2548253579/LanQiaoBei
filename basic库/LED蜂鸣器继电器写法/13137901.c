#include <STC15F2K60S2.H>

bit L1,L2,L3,L4,L5,L6,L7,L8;//LED标志、
bit relay;
int time_1s,time;

void led_set (void)//led控制
{
P0=0XFF;//消影
P2=0X80;//选择Y4C
if(L1==1){P00=0;}if(L1==0){P00=1;}
if(L2==1){P01=0;}if(L2==0){P01=1;}	
if(L3==1){P02=0;}if(L3==0){P02=1;}
if(L4==1){P03=0;}if(L4==0){P03=1;}
if(L5==1){P04=0;}if(L5==0){P04=1;}	
if(L6==1){P05=0;}if(L6==0){P05=1;}
if(L7==1){P06=0;}if(L7==0){P06=1;}
if(L8==1){P07=0;}if(L8==0){P07=1;}	
P2=0X00;
}

void buzz_set (void)
{
P0=0x00;
P2=0xa0;
if(relay==1){P04=1;}if(relay==0){P04=0;}	
P2=0X00;	
}


void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	EA=1;
	ET1=1;
}




int main ()
{
	P2=0X80;P0=0XFF;P2=0X00;//关闭LED
	P2=0XA0;P0=0X00;P2=0X00;//关闭蜂鸣器继电器
Timer1Init();	
	
while (1)
{

if(time==1)L1=1;	
if(time==2)L2=1;	
if(time==3)L3=1;	
if(time==4)L4=1;	
if(time==5)L5=1;	
if(time==6)L6=1;	
if(time==7)L7=1;	
if(time==8)L8=1;	

if(time==11)L8=0;	
if(time==12)L7=0;	
if(time==13)L6=0;	
if(time==14)L5=0;	
if(time==15)L4=0;	
if(time==16)L3=0;	
if(time==17)L2=0;	
if(time==18)L1=0;

if(time==20)relay=1;	
if(time==23){relay=0;time=0;}
	
}
}

void Server (void) interrupt 3
{
time_1s++;if(time_1s==1000){time_1s=0;time++;}
led_set();
buzz_set ();
}







