#include "STC15F2K60S2.H"
#include "INTRINS.H"
#include "IIC.H"
#define TX P10 
#define RX P11

bit L1,L2,L3,L4,L5,L6,L7,L8,RELAY,BUZZER,L3_BLINK,L4_BLINK,DISPLAY=1,TIME1_BLINK,TIME2_BLINK,start_trans,loading_mode,time1_display=1,time2_display=1,transporting;
unsigned char SMG[8],key_value,tick_schedule,tick_schedule1;
unsigned char goods_type,trans_time_1,trans_time_2,trans_time_left,display_mode=1;
unsigned int distance,voltage;

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



void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;
  EA  = 1;	
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

void hardware_control (void)
{
P0=0xff;
P2=0X80;
P00=~L1;P01=~L2;P02=~L3;P03=~L4;P04=~L5;P05=~L6;P06=~L7;P07=~L8;
P2=0X00;
P0=0x00;
P2=0XA0;
P04=RELAY;P06=BUZZER;
P2=0X00;
}

unsigned int ceju(void)//测量物体距离
{
	unsigned char a=10;
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

void smg(unsigned char wei,unsigned char duan)
{
P0=0xff;
P2=0XC0;P0=T_COM[wei];P2=0X00;
P2=0XE0;P0=~t_display[duan];P2=0X00;
}

void display (unsigned char mode)
{
if(DISPLAY==1)
{
if(mode==1)
{
SMG[0]=mode;SMG[1]=16;SMG[2]=16;SMG[3]=distance/10%10;SMG[4]=distance%10;SMG[5]=16;SMG[6]=16;SMG[7]=goods_type;
}
if(mode==2)
{
SMG[0]=mode;SMG[1]=16;SMG[2]=16;SMG[3]=16;SMG[4]=16;SMG[5]=16;SMG[6]=trans_time_left/10%10;SMG[7]=trans_time_left%10;
}
}
if(DISPLAY==0)
{
if(mode==3)
{
SMG[0]=mode;SMG[1]=16;SMG[2]=16;if(time1_display==1){SMG[3]=trans_time_1/10%10;SMG[4]=trans_time_1%10;}else{SMG[3]=16;SMG[4]=16;}SMG[5]=16;if(time2_display==1){SMG[6]=trans_time_2/10%10;SMG[7]=trans_time_2%10;}else{SMG[6]=16;SMG[7]=16;}
}	
else
{
SMG[0]=16;SMG[1]=16;SMG[2]=16;SMG[3]=16;SMG[4]=16;SMG[5]=16;SMG[6]=16;SMG[7]=16;
}
}

}

void key_task (void)
{

}

int keboard (void)
{
static unsigned char line;
unsigned char	key_val,key_trig;
key_trig=0;
if(line==0)	
{
P30=0;P31=1;P32=1;P33=1;
if(P34==0){while(!P34){key_task();}key_trig=1;key_val=4;}	
if(P35==0){while(!P35){key_task();}key_trig=1;key_val=3;}	
if(P42==0){while(!P42){key_task();}key_trig=1;key_val=2;}	
if(P44==0){while(!P44){key_task();}key_trig=1;key_val=1;}	
}	
if(line==1)	
{
P30=1;P31=0;P32=1;P33=1;
if(P34==0){while(!P34){key_task();}key_trig=1;key_val=8;}	
if(P35==0){while(!P35){key_task();}key_trig=1;key_val=7;}	
if(P42==0){while(!P42){key_task();}key_trig=1;key_val=6;}	
if(P44==0){while(!P44){key_task();}key_trig=1;key_val=5;}	
}	
if(line==2)	
{
P30=1;P31=1;P32=0;P33=1;
if(P34==0){while(!P34){key_task();}key_trig=1;key_val=12;}	
if(P35==0){while(!P35){key_task();}key_trig=1;key_val=11;}	
if(P42==0){while(!P42){key_task();}key_trig=1;key_val=10;}	
if(P44==0){while(!P44){key_task();}key_trig=1;key_val=9;}	
}	
if(line==3)	
{
P30=1;P31=1;P32=1;P33=0;
if(P34==0){while(!P34){key_task();}key_trig=1;key_val=16;}	
if(P35==0){while(!P35){key_task();}key_trig=1;key_val=15;}	
if(P42==0){while(!P42){key_task();}key_trig=1;key_val=14;}	
if(P44==0){while(!P44){key_task();}key_trig=1;key_val=13;}	
}

line++;if(line==4){line=0;}
if(key_trig==0){return 99;}
else {return key_val;}
}

void read_time (void)
{
static unsigned char lock;
Delay4ms();
lock=read_24c02(0x07);
if(lock==0x07)	
{
Delay4ms();
trans_time_1=read_24c02(0x01);
Delay4ms();
trans_time_2=read_24c02(0x02);
}
else
{
trans_time_1=2;
trans_time_2=4;
}

}

void write_time (unsigned char time1,unsigned char time2)
{
Delay4ms();
write_24c02(0x01,time1);
Delay4ms();
write_24c02(0x02,time2);
Delay4ms();
if(time1!=0x02||time2!=0x04)
{
write_24c02(0x07,0x07);
}
}

void voltage_config (void)
{
if(voltage>=0&&voltage<1){L1=1;DISPLAY=0;}else{L1=0;DISPLAY=1;}
if(voltage>=1&&voltage<4){L2=1;loading_mode=1;}else{L2=0;loading_mode=0;}
if(voltage>=4){BUZZER=1;L3_BLINK=1;}else{BUZZER=0;L3_BLINK=0;}
}

void control_center (void)
{
static unsigned char config_mode;
	
if(loading_mode==1)
{
if(key_value==13&&transporting==0)//开启传送
{
display_mode=2;
start_trans=1;
transporting=1;
}

if(key_value==9&&transporting==1)//紧急暂停（仅在传送过程中有效）
{
key_value=99;
RELAY=~RELAY;L4_BLINK=~L4_BLINK;start_trans=~start_trans;
}	
}
if(DISPLAY==0)
{
if(key_value==5)//设置按键
{
key_value=99;
config_mode++;
}
if(config_mode==1)	{display_mode=3;TIME1_BLINK=1;TIME2_BLINK=0;if(key_value==1){key_value=99;trans_time_1++;if(trans_time_1==11){trans_time_1=1;}}}
if(config_mode==2)	{display_mode=3;TIME1_BLINK=0;TIME2_BLINK=1;if(key_value==1){key_value=99;trans_time_2++;if(trans_time_2==11){trans_time_2=1;}}}
if(config_mode==3)	{display_mode=1;TIME1_BLINK=0;TIME2_BLINK=0;config_mode=0;write_time (trans_time_1,trans_time_2);}	

}

}


void task_schedule(void)
{
if(tick_schedule>=40)	//10ms执行一次
{tick_schedule=0;
voltage=(int)((((float)read_adc(0x03))/255)*5);
control_center ();
}

if(tick_schedule1>=100)	//200ms执行一次
{tick_schedule1=0;
distance=ceju();
}


key_value=keboard ();
}


int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;
P2=0XA0;P0=0X00;P2=0X00;
P34=1;P35=1;P42=1;P44=1;
read_time ();
Timer1Init();
while(1)
{
task_schedule();
}
}




void server (void) interrupt 3
{
static unsigned char tick_8ms;
static unsigned int tick_300ms,tick_500ms_L4,tick_500ms1,tick_500ms2,trans_tick1,trans_tick2;
tick_schedule++;tick_schedule1++;
if(L3_BLINK==1){tick_300ms++;if(tick_300ms==300){tick_300ms=0;L3=~L3;}}else{tick_300ms=0;L3=0;}
if(L4_BLINK==1){tick_500ms_L4++;if(tick_500ms_L4==500){tick_500ms_L4=0,L4=~L4;}}else{tick_500ms_L4=0;L4=0;}
if(TIME1_BLINK==1){tick_500ms1++;if(tick_500ms1==500){tick_500ms1=0;time1_display=~time1_display;}}else{tick_500ms1=0;time1_display=1;}
if(TIME2_BLINK==1){tick_500ms2++;if(tick_500ms2==500){tick_500ms2=0;time2_display=~time2_display;}}else{tick_500ms2=0;time2_display=1;}
if(goods_type==1&&start_trans==1){trans_tick1++;RELAY=1;trans_time_left=(int)((float)(trans_time_1*1000-trans_tick1)/1000);if(trans_tick1==trans_time_1*1000){trans_tick1=0;start_trans=0;transporting=0;RELAY=0;}}
if(goods_type==2&&start_trans==1){trans_tick2++;RELAY=1;trans_time_left=(int)((float)(trans_time_2*1000-trans_tick2)/1000);if(trans_tick2==trans_time_2*1000){trans_tick2=0;start_trans=0;transporting=0;RELAY=0;}}

tick_8ms++;if(tick_8ms==8){tick_8ms=0;}
smg(tick_8ms,SMG[tick_8ms]);
if(distance<=30){goods_type=1;}else{goods_type=2;}
display (display_mode);
hardware_control ();
voltage_config ();
}
