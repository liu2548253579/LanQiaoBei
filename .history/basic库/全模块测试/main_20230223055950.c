#include "stc15f2k60s2.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
#include "intrins.h"
#include "stdio.h"

#define TX P10
#define RX P11

char TASK_RUN=1;//任务运行标志位（对应任务1-9）
bit init_flag=1;//初始化标志位


unsigned char code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

bit LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8,BUZZER,RELAY;//LED、蜂鸣器、继电器控制标志位

unsigned char SMG[8],key_value;//数码管，键值
unsigned int task1_tick,task2_tick,task3_tick,task4_tick,task5_tick,task6_tick,task7_tick,task8_tick,task9_tick;//各个任务时间标志位
int tempreture;//温度（乘了100倍）
int fre;//频率



//基础配置LED、蜂鸣器、继电器
void BASIC (void)
{
	P0=0xFF;P2=0x80;//消隐
	P00=~LED1;P01=~LED2;P02=~LED3;P03=~LED4;P04=~LED5;P05=~LED6;P06=~LED7;P07=~LED8;//同步标志位状态
	P2=0x00;
		
	P0=0x00;P2=0xA0;//消隐
	P04=RELAY;P06=BUZZER;//同步标志位状态
	P2=0x00;	
}




//数码管刷新

void smg (unsigned char wei,unsigned char duan)
{
P0=0xff;
P2=0xc0;P0=T_COM[wei];P2=0x00;
P2=0xe0;P0=~t_display[duan];P2=0x00;
}





//任务定时器

void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04; 
	EA=1;
}






//配置串口发送

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




//配置DS1302时钟

void config_ds1302 (unsigned char shi,unsigned char fen,unsigned char miao)
{
Write_Ds1302_Byte(0x8e,0x00);
Write_Ds1302_Byte(0x80,miao);
Write_Ds1302_Byte(0x82,fen);
Write_Ds1302_Byte(0x84,shi);	
Write_Ds1302_Byte(0x8e,0x80);
}

void display_ds1302 (void)
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





//读取温度

void display_temp (void)
{
tempreture=(int)(read_temp()*100);	
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=tempreture/1000%10;
SMG[5]=(tempreture/100%10)+32;
SMG[6]=tempreture/10%10;
SMG[7]=tempreture%10;	
}




//读取电压

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

void display_raw_adc (unsigned char addr)
{
unsigned int vol;
vol=adc(addr);
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=vol/1000%10;
SMG[5]=vol/100%10;
SMG[6]=vol/10%10;
SMG[7]=vol%10;
}



//读取EEPROM

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


//写入24c02数据延时

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






//超声波测距

void CejuInit(void)		//12微秒@12.000MHz
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
CejuInit();	
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
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=17;
SMG[5]=distance/100%10;
SMG[6]=distance/10%10;
SMG[7]=distance%10;
}


//按键读取

void read_key (void)
{
char trig=0;
unsigned char key_val;	
trig=0;	
P44=0;P42=1;P35=1;P34=1;
if(P30==0){while(!P30){}trig=1;key_val=1;}
if(P31==0){while(!P31){}trig=1;key_val=5;}	
if(P32==0){while(!P32){}trig=1;key_val=9;}	
if(P33==0){while(!P33){}trig=1;key_val=13;}
P44=1;P42=0;P35=1;P34=1;
if(P30==0){while(!P30){}trig=1;key_val=2;}
if(P31==0){while(!P31){}trig=1;key_val=6;}	
if(P32==0){while(!P32){}trig=1;key_val=10;}	
if(P33==0){while(!P33){}trig=1;key_val=14;}
P44=1;P42=1;P35=0;P34=1;
if(P30==0){while(!P30){}trig=1;key_val=3;}
if(P31==0){while(!P31){}trig=1;key_val=7;}	
if(P32==0){while(!P32){}trig=1;key_val=11;}	
if(P33==0){while(!P33){}trig=1;key_val=15;}
P44=1;P42=1;P35=1;P34=0;
if(P30==0){while(!P30){}trig=1;key_val=4;}
if(P31==0){while(!P31){}trig=1;key_val=8;}	
if(P32==0){while(!P32){}trig=1;key_val=12;}	
if(P33==0){while(!P33){}trig=1;key_val=16;}

if(!trig){key_val=99;}
key_value=key_val;
}


void display_key (void)
{
static int num_tick;
int dis_num;
	
if(key_value!=99)
{
if(key_value==15)
{
num_tick++;
	if(num_tick>=99){num_tick=99;}
}
else if(key_value==16)
{
num_tick--;
	if(num_tick<=0){num_tick=0;}
}
else 
{
dis_num=key_value;
}
}

SMG[0]=dis_num/10%10;
SMG[1]=dis_num%10;
SMG[2]=17;
SMG[3]=17;
SMG[4]=17;
SMG[5]=17;
SMG[6]=num_tick/10%10;
SMG[7]=num_tick%10;	
}




//频率采集

void FrequenceInit(void)		//100微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD = 0x04;		//设置定时器模式
	TL0 = 0x50;		//设置定时初值
	TH0 = 0xFB;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}



void Frequence (void)
{
static int tick_500ms;
tick_500ms++;
if(tick_500ms==500)
{
TR0=0;
tick_500ms=0;
fre=TH0<<8;
fre+=TL0;
TH0=0;
TL0=0;
TR0=1;	
}	
}

void display_frequence (void)
{
	SMG[0]=15;
	SMG[1]=16;
	SMG[2]=16;
	if(fre>9999){SMG[3]=fre/10000%10;}else{SMG[3]=16;}
	if(fre>999)SMG[4]=fre/1000%10;else{SMG[4]=16;}
	if(fre>99)SMG[5]=fre/100%10;else{SMG[5]=16;}
	if(fre>9)SMG[6]=fre/10%10;else{SMG[6]=16;}
	if(fre>0)SMG[7]=fre%10;else{SMG[7]=16;}
}




//任务1初始化
void task1_init (void)
{
//设置初始时间
config_ds1302(0x23,0x59,0x58);
}

//任务1循环
void task1_loop (void)
{
if(task1_tick>=10)
{task1_tick=0;
display_ds1302();
}
}





//任务2循环
void task2_loop (void)
{
if(task2_tick>=500)
{task2_tick=0;
display_temp();
}
}




//任务3初始化
void task3_init (void)
{
FrequenceInit();
}

//任务3循环
void task3_loop (void)
{
Frequence();
if(task3_tick>=10)
{task3_tick=0;
display_frequence();
}
}



//任务4初始化
void task4_init (void)
{
P30=1;P31=1;P32=1;P33=1;
}

//任务4循环
void task4_loop (void)
{
display_key();
}



//任务5循环
void task5_loop (void)
{
if(task5_tick>=10)
{task5_tick=0;
display_distance ();
}
}

//任务6初始化
void task6_init (void)
{
UartInit();//9600bps@12.000MHz
}

//任务6循环
void task6_loop (void)
{
if(task6_tick>=1000)
{task6_tick=0;
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=17;
SMG[5]=17;
SMG[6]=17;
SMG[7]=17;
tempreture=(int)(read_temp()*100);	
printf("%d%d.%d%d\r\n",tempreture/1000%10,tempreture/100%10,tempreture/10%10,tempreture%10);
}
}


//任务7循环
void task7_loop (void)
{
if(task7_tick>=200)
{task7_tick=0;
display_raw_adc(0x01);
}
}

//任务8循环
void task8_loop (void)
{
static int count_8;	
	
if(task8_tick>=1000)
{task8_tick=0;
if(count_8==0){LED1=1;LED2=0;LED3=0;LED4=0;LED5=0;LED6=0;LED7=0;LED8=0;RELAY=1;}
if(count_8==1){LED1=0;LED2=1;LED3=0;LED4=0;LED5=0;LED6=0;LED7=0;LED8=0;RELAY=0;}
if(count_8==2){LED1=0;LED2=0;LED3=1;LED4=0;LED5=0;LED6=0;LED7=0;LED8=0;RELAY=1;}
if(count_8==3){LED1=0;LED2=0;LED3=0;LED4=1;LED5=0;LED6=0;LED7=0;LED8=0;RELAY=0;}
if(count_8==4){LED1=0;LED2=0;LED3=0;LED4=0;LED5=1;LED6=0;LED7=0;LED8=0;RELAY=1;}
if(count_8==5){LED1=0;LED2=0;LED3=0;LED4=0;LED5=0;LED6=1;LED7=0;LED8=0;RELAY=0;}
if(count_8==6){LED1=0;LED2=0;LED3=0;LED4=0;LED5=0;LED6=0;LED7=1;LED8=0;RELAY=1;}
if(count_8==7){LED1=0;LED2=0;LED3=0;LED4=0;LED5=0;LED6=0;LED7=0;LED8=1;RELAY=0;}
SMG[0]=17;
SMG[1]=17;
SMG[2]=17;
SMG[3]=17;
SMG[4]=17;
SMG[5]=17;
SMG[6]=17;
SMG[7]=count_8%10;
count_8++;if(count_8==8){count_8=0;}
}
}


//任务9初始化
void task9_init (void)
{
write_24c02(0x01,0x05);
Delay4ms();
}

//任务9循环
void task9_loop (void)
{
if(task9_tick>=1000)
{task9_tick=0;
display_24c02 (0x01);
}
}


//控制任务切换
void task_control_center (void)
{
read_key ();//读取键盘
if(key_value==14)
{
TASK_RUN++;init_flag=1;
}
if(TASK_RUN>=10){TASK_RUN=1;}	
	
}



int main (void)
{
//关闭外设
P2=0X80;P0=0XFF;P2=0X00;
P2=0XA0;P0=0X00;P2=0X00;
	

//任务定时器初始化
Timer2Init();	
	
while(1)
{
	
if(init_flag)
{
//非任务6关闭定时器1
if(TASK_RUN!=6)
{
TR1=0;
}		
	
//非任务8关闭外设
if(TASK_RUN!=8)
{
P2=0X80;P0=0XFF;P2=0X00;
P2=0XA0;P0=0X00;P2=0X00;
}		
	
	
	
//任务1初始化	
if(TASK_RUN==1){task1_init ();}
	
//任务3初始化	
if(TASK_RUN==3){task3_init ();}

//任务4初始化	
if(TASK_RUN==4){task4_init ();}

//任务6初始化	
if(TASK_RUN==6){task6_init ();}

//任务9初始化	
if(TASK_RUN==9){task9_init ();}


init_flag=0;
}	
	
	
//任务1循环	
if(TASK_RUN==1){task1_loop();}
//任务2循环	
if(TASK_RUN==2){task2_loop();}
//任务4循环	
if(TASK_RUN==4){task4_loop();}
//任务5循环	
if(TASK_RUN==5){task5_loop();}
//任务6循环	
if(TASK_RUN==6){task6_loop();}
//任务7循环	
if(TASK_RUN==7){task7_loop();}
//任务8循环	
if(TASK_RUN==8){task8_loop();}
//任务9循环	
if(TASK_RUN==9){task9_loop();}
	
task_control_center ();

}	
}





void server (void) interrupt 12
{
//数码管刷新部分以及基础控制部分
static unsigned char tick_8ms;
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}
smg(tick_8ms,SMG[tick_8ms]);
BASIC ();

//任务1计数器自增，以及防止溢出
task1_tick++;if(task1_tick>2000){task1_tick=0;}
//任务2计数器自增，以及防止溢出
task2_tick++;if(task2_tick>2000){task2_tick=0;}
//任务3计数器自增，以及防止溢出
task3_tick++;if(task3_tick>2000){task3_tick=0;}
//任务4计数器自增，以及防止溢出
task4_tick++;if(task4_tick>2000){task4_tick=0;}
//任务5计数器自增，以及防止溢出
task5_tick++;if(task5_tick>2000){task5_tick=0;}
//任务6计数器自增，以及防止溢出
task6_tick++;if(task6_tick>2000){task6_tick=0;}
//任务7计数器自增，以及防止溢出
task7_tick++;if(task7_tick>2000){task7_tick=0;}
//任务8计数器自增，以及防止溢出
task8_tick++;if(task8_tick>2000){task8_tick=0;}
//任务9计数器自增，以及防止溢出
task9_tick++;if(task9_tick>2000){task9_tick=0;}


//任务3循环	
if(TASK_RUN==3){task3_loop();}
}

