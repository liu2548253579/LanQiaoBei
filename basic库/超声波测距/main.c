#include "stc15f2k60s2.h"


#define TX P10 
#define RX P11

unsigned char tick_8ms,tick_20ms,SMG[8];//8ms时间标志 20ms时间标志 数码管刷新内容
unsigned int juli=999;//测距距离 

unsigned char code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

void Timer0Init(void)		//12微秒@12.000MHz
 {
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}



unsigned int ceju(void)//测量物体距离
{
	unsigned char a=10;//脉冲数量-长度
	unsigned int juli=0;
	Timer0Init();
	TX=0;//关闭发送脉冲
	
	while(a--)//发送十个脉冲
	{
		while(!TF0);//等待溢出	
		TX=1;//开始发送脉冲
		TF0=0;//清零溢出标志位
	}
		TR0=0;//关闭定时器0
	
	  TH0=0;//定时器0高位清零
  	TL0=0;//定时器0低位清零
	
	  TR0=1;//打开定时器0开始计时
	
	while(RX&&!TF0);//当RX接收到脉冲或者TF0溢出时继续
	if(TF0==1){juli=999;}//TF0溢出距离设为无穷大即为999cm
	else{juli=((TH0<<8)+TL0)*0.017;}//根据距离时间公式计算实际距离
	
	TR0=0;//关闭定时器0		
	return juli;
}



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

void display_distance (void)
{
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=17;
SMG[5]=juli/100%10;
SMG[6]=juli/10%10;
SMG[7]=juli%10;
}

void Task (void)
{
if(tick_20ms>=20)
{
tick_20ms=0;
	juli=ceju();
	display_distance();
}	
}


int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//关闭LED外设
P2=0XA0;P0=0X00;P2=0X00;//关闭继电器蜂鸣器外设

Timer1Init();//任务调度定时器初始化
while(1)
{	
Task();
}
	
}

void server () interrupt 3
{
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//数码管扫描标志位
tick_20ms++;//任务调度20ms测一次距离

smg(tick_8ms,SMG[tick_8ms]);//数码管更新	
}
