#include "stc15f2k60s2.h"

unsigned char key_value=99;//键值 
unsigned char tick_8ms,tick_2ms,SMG[8];//8ms时间标志 数码管刷新内容 

unsigned char code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码


void smg (unsigned char duan,unsigned char wei)//数码管刷新单元
{
P0=0XFF;
P2=0XC0;P0=T_COM[duan];P2=0X00;
P2=0XE0;P0=~t_display[wei];P2=0X00;
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

void key_task (void)//按键按下执行的任务
{

}


unsigned char read_keyboard (void)//返回读取到的按键键值若无按键按下则返回99
{
static unsigned char keyboard_tick;//轮询标志位
unsigned char	key_val,key_trig=0;//键值暂存，触发标志

key_trig=0;	//清零触发
	
if(keyboard_tick==0)
{
P30=0;P31=1;P32=1;P33=1;
if(P44==0){while(!P44){key_task();}key_val=0;key_trig=1;}	
if(P42==0){while(!P42){key_task();}key_val=1;key_trig=1;}	
if(P35==0){while(!P35){key_task();}key_val=2;key_trig=1;}	
if(P34==0){while(!P34){key_task();}key_val=3;key_trig=1;}	
}	


if(keyboard_tick==1)
{
P30=1;P31=0;P32=1;P33=1;	
if(P44==0){while(!P44){key_task();}key_val=4;key_trig=1;}	
if(P42==0){while(!P42){key_task();}key_val=5;key_trig=1;}	
if(P35==0){while(!P35){key_task();}key_val=6;key_trig=1;}	
if(P34==0){while(!P34){key_task();}key_val=7;key_trig=1;}	
}

if(keyboard_tick==2)
{
P30=1;P31=1;P32=0;P33=1;
if(P44==0){while(!P44){key_task();}key_val=8;key_trig=1;}	
if(P42==0){while(!P42){key_task();}key_val=9;key_trig=1;}	
if(P35==0){while(!P35){key_task();}key_val=10;key_trig=1;}
if(P34==0){while(!P34){key_task();}key_val=11;key_trig=1;}	
}

if(keyboard_tick==3)
{
P30=1;P31=1;P32=1;P33=0;
if(P44==0){while(!P44){key_task();}key_val=12;key_trig=1;}	
if(P42==0){while(!P42){key_task();}key_val=13;key_trig=1;}	
if(P35==0){while(!P35){key_task();}key_val=14;key_trig=1;}
if(P34==0){while(!P34){key_task();}key_val=15;key_trig=1;}	

}
keyboard_tick=keyboard_tick+1;//轮询键盘

if(keyboard_tick==4){keyboard_tick=0;}


if(key_trig){return key_val;}//如果触发则返回键值
else{return 99;}//如果未触发则返回99
}

void task_schedule (void)
{
	if(tick_2ms>=2)
	{
		tick_2ms=0;
		key_value=read_keyboard();//读取键值
	}

}



int main (void)
{
int key_num;	
P2=0X80;P0=0XFF;P2=0X00;//关闭LED外设
P2=0XA0;P0=0X00;P2=0X00;//关闭继电器蜂鸣器外设
P34=1;P35=1;P42=1;P44=1;//初始化按键引脚
Timer1Init();//任务调度定时器初始化
	while(1)
	{	
		task_schedule();
		if(key_value==1){SMG[0]=key_value;}	//按键触发改变键值
	}
		
}

void server () interrupt 3
{	
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//数码管扫描标志位
smg(tick_8ms,SMG[tick_8ms]);//数码管更新	
tick_2ms++;
}
