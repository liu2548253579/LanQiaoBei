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



//按键读取（不要把它放定时器里，在按键按下时会重复执行key_task()）
unsigned char  read_keyboard (void)
{
	bit trig=0;//按键触发标志位
	unsigned char key_val;//键值暂存位
	trig=0;//触发标志位置零（可以不加）
	P44=0;P42=1;P35=1;P34=1;//扫描第一列
		if(P30==0){while(!P30){key_task();}trig=1;key_val=1;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=5;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=9;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=13;}
	P44=1;P42=0;P35=1;P34=1;//扫描第二列
		if(P30==0){while(!P30){key_task();}trig=1;key_val=2;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=6;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=10;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=14;}
	P44=1;P42=1;P35=0;P34=1;//扫描第三列
		if(P30==0){while(!P30){key_task();}trig=1;key_val=3;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=7;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=11;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=15;}
	P44=1;P42=1;P35=1;P34=0;//扫描第四列
		if(P30==0){while(!P30){key_task();}trig=1;key_val=4;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=8;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=12;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=16;}

		if(!trig){key_val=99;}//若无按键触发则键值为99
		return key_val;//更新键值
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
P2=0X80;P0=0XFF;P2=0X00;//关闭LED外设
P2=0XA0;P0=0X00;P2=0X00;//关闭继电器蜂鸣器外设
P34=1;P35=1;P42=1;P44=1;//初始化按键引脚
Timer1Init();//任务调度定时器初始化
	while(1)
	{	
		task_schedule();
				key_value=read_keyboard();//读取键值
		if(key_value!=99){SMG[0]=key_value;}	//按键触发改变键值
	}
		
}

void server () interrupt 3
{	
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//数码管扫描标志位
smg(tick_8ms,SMG[tick_8ms]);//数码管更新	
tick_2ms++;
}
