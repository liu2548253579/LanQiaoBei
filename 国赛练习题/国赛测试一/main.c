#include "stc15f2k60s2.h"
#include "iic.h"
#include "ds1302.h"
#include "intrins.h"

#define TX P10 
#define RX P11

bit relay,buzzer;//继电器，蜂鸣器控制标志位
bit working_mode=1;//为零是自动门为一是密码门
bit door_5s,buzzer_3s;//蜂鸣器和继电器的延时启动标志位
unsigned int gao=654,di=321;//用于存储密码的高三位和低三位
unsigned char display_mode,logic_mode,key_enter_flag,key_value,error_times;//显示模式 逻辑模式 按键输入标志位（位数） 键值 错误次数
unsigned char tick_8ms,tick_10ms,SMG[8],shi,fen,miao;//8ms时间标志 10ms时间标志 数码管刷新内容 时 分 秒
unsigned int juli=999;//测距距离 
unsigned int password[6]={16,16,16,16,16,16},password_gao,password_di;//六位密码输入暂存位 暂存高三位 暂存低三位

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

void hardware_control (void)//硬件控制
{
P0=0X00;
P2=0XA0;
if(buzzer==1){P06=1;}
if(buzzer==0){P06=0;}

if(relay==1){P04=1;}
if(relay==0){P04=0;}
P2=0X00;
}


void init_ds1302 (unsigned char shi_set,unsigned char fen_set,unsigned char miao_set)//设置ds1302时间并更新到全局变量
{
Write_Ds1302_Byte(0x8e,0x00);
Write_Ds1302_Byte(0x80,miao_set);	
Write_Ds1302_Byte(0x82,fen_set);
Write_Ds1302_Byte(0x84,shi_set);	
Write_Ds1302_Byte(0x8e,0x80);	
shi=Read_Ds1302_Byte(0x85);
fen=Read_Ds1302_Byte(0x83);
miao=Read_Ds1302_Byte(0x81);
}

void read_ds1302 (void)//读取并更新ds1302时钟信息
{
	EA=0;
shi=Read_Ds1302_Byte(0x85);
fen=Read_Ds1302_Byte(0x83);
miao=Read_Ds1302_Byte(0x81);
	EA=1;
}

void display_ds1302 (void)//显示时间
{
SMG[0]=shi/16;
SMG[1]=shi%16;
SMG[2]=17;
SMG[3]=fen/16;
SMG[4]=fen%16;
SMG[5]=17;
SMG[6]=miao/16;
SMG[7]=miao%16;
}

void display_enter (void)//显示输密码的界面
{
SMG[0]=17;
SMG[1]=17;
SMG[2]=password[0];
SMG[3]=password[1];
SMG[4]=password[2];
SMG[5]=password[3];
SMG[6]=password[4];
SMG[7]=password[5];
}

void display_config (void)//显示确认旧密码界面
{
SMG[0]=16;
SMG[1]=17;
SMG[2]=password[0];
SMG[3]=password[1];
SMG[4]=password[2];
SMG[5]=password[3];
SMG[6]=password[4];
SMG[7]=password[5];
}

void display_new (void)//显示输入新密码界面
{
SMG[0]=17;
SMG[1]=16;
SMG[2]=password[0];
SMG[3]=password[1];
SMG[4]=password[2];
SMG[5]=password[3];
SMG[6]=password[4];
SMG[7]=password[5];
}

void display_change (void)//密码模式三种状态切换
{
if(display_mode==0){display_enter();}
if(display_mode==1){display_config();}
if(display_mode==2){display_new();}
}

void Delay4ms()		//@12.000MHz   4ms延时给24c02用
{
	unsigned char i, j;

	i = 47;
	j = 174;
	do
	{
		while (--j);
	} while (--i);
}

unsigned char config_24c02_state (unsigned char mode)//设置或者读取24c02密码是否被改写过
{
if(mode==0){Delay4ms();write_24c02 (0x07,0);}
if(mode==1){Delay4ms();write_24c02 (0x07,7);}	
Delay4ms();	
return read_24c02(0x07);	
}


//将密码写入24c02
void write_password (unsigned char a1,unsigned char a2,unsigned char a3,unsigned char a4,unsigned char a5,unsigned char a6)
{
write_24c02 (0x01,a1);
Delay4ms();
write_24c02 (0x02,a2);	
Delay4ms();
write_24c02 (0x03,a3);
Delay4ms();
write_24c02 (0x04,a4);
Delay4ms();
write_24c02 (0x05,a5);
Delay4ms();
write_24c02 (0x06,a6);	
Delay4ms();
}


//将密码从24c02更新到gao和di
void read_password (void)
{
unsigned char a1,a2,a3,a4,a5,a6;
if(config_24c02_state(2)==7)//如果密码被改写过则读取
{
a1=read_24c02(0x01);
a2=read_24c02(0x02);
a3=read_24c02(0x03);
a4=read_24c02(0x04);
a5=read_24c02(0x05);
a6=read_24c02(0x06);	
Delay4ms();
gao=a1*100+a2*10+a3;
di=a4*100+a5*10+a6;
}
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
if(P35==0){while(!P35){key_task();}key_val=10;key_trig=1;}//	设置
if(P34==0){while(!P34){key_task();}key_val=11;key_trig=1;}	//复位
}

if(keyboard_tick==3)
{
P30=1;P31=1;P32=1;P33=0;
if(P35==0){while(!P35){key_task();}key_val=12;key_trig=1;}//	确认
if(P34==0){while(!P34){key_task();}key_val=13;key_trig=1;}	//退出
}
keyboard_tick=keyboard_tick+1;//轮询键盘

if(keyboard_tick==4){keyboard_tick=0;}


if(key_trig){return key_val;}//如果触发则返回键值
else{return 99;}//如果未触发则返回错误码99
}

void clean_password (void)//清除输入的位数和数字
{
password[0]=16,password[1]=16,password[2]=16,password[3]=16,password[4]=16,password[5]=16;
key_enter_flag=0;
}

void control_center (void)//系统逻辑中心
{
if(working_mode==0)//自动门
{
display_ds1302 ();//显示时间界面
if(juli<30){relay=1;door_5s=1;}//如果距离少于30cm则开门
}

if(working_mode==1)//密码门
{
display_change();//显示不同的密码门显示
if(key_value==11){write_password (6,5,4,3,2,1);read_password();	config_24c02_state(0);}//复位 设置密码为654321 读取密码 将24c02状态置为未改写




if(logic_mode==0)//密码输入模式
{
display_mode=0;//切换密码模式显示为输入显示
if(key_value<=9){password[key_enter_flag]=key_value;key_enter_flag=key_enter_flag+1;if(key_enter_flag>=5){key_enter_flag=5;}}//如果键值小于9（为数字）则 将对应的位数赋值给对应的数组 并限制输入六位数

if(key_value==12)//确认
{
password_gao=password[0]*100+password[1]*10+password[2];//输入高三位密码赋值给暂存高三位
password_di=password[3]*100+password[4]*10+password[5];//输入低三位密码赋值给暂存低三位
if(password_gao==gao)//判断密码是否正确
{
if(password_di==di){clean_password();error_times=0;relay=1;door_5s=1;}//清除输入 将错误次数置零 打开继电器 开启延时5s
}

else{error_times++;clean_password();}//密码错误 错误次数加一 清除输入
if(error_times>=3){error_times=0;buzzer=1;buzzer_3s=1;}//错误次数大于三次 清零错误次数 打开蜂鸣器 开启延时3s
}

}

if(key_value==10)//设置
{
clean_password();//清除输入
logic_mode=1;//切换为密码设置模式
}


if(logic_mode==1)//密码设置模式
{
display_mode=1;//切换密码模式显示为设置显示
if(key_value==13){logic_mode=0;clean_password();}//退出 切换为密码输入模式 清除输入
else
{
if(key_value<=9){password[key_enter_flag]=key_value;key_enter_flag=key_enter_flag+1;if(key_enter_flag>=5){key_enter_flag=5;}}//如果键值小于9（为数字）则 将对应的位数赋值给对应的数组 并限制输入六位数
if(key_value==12)//确认
{
password_gao=password[0]*100+password[1]*10+password[2];//输入高三位密码赋值给暂存高三位
password_di=password[3]*100+password[4]*10+password[5];//输入低三位密码赋值给暂存低三位
if(password_gao==gao)//判断密码是否正确
{
if(password_di==di){clean_password();logic_mode=2;error_times=0;key_value=99;}//清除输入 切换为新密码输入模式 将错误次数置零 将键值暂时设置为99以免快速确认两次
}

else{error_times++;clean_password();}//密码错误 错误次数加一 清除输入
if(error_times>=3){error_times=0;buzzer=1;buzzer_3s=1;logic_mode=0;}//错误次数大于三次 清零错误次数 打开蜂鸣器 开启延时3s
}
}
}

if(logic_mode==2)//新密码配置
{
display_mode=2;//切换密码模式显示为新密码输入显示
if(key_value==13){logic_mode=0;clean_password();}//退出
else
{
if(key_value<=9){password[key_enter_flag]=key_value;key_enter_flag=key_enter_flag+1;if(key_enter_flag>=5){key_enter_flag=5;}}//如果键值小于9（为数字）则 将对应的位数赋值给对应的数组 并限制输入六位数
if(key_value==12)//确认
{
write_password (password[0],password[1],password[2],password[3],password[4],password[5]);//将对应的新密码逐个写入对应的位置
read_password();//更新密码到数组
config_24c02_state(1);//更改密码改写状态为经过修改
clean_password();//清除输入
logic_mode=0;	//切换为密码输入模式
}
}
}

}

}

void mode_control (void)//根据时间切换工作模式
{
if(shi>=0x07){if(shi<=0x21){working_mode=0;}}//（7点到21点59分59秒为自动门）
else{working_mode=1;}//（其余时间为密码门）
}

void task_schedule (void)//定时器任务调度
{
if(tick_10ms>=10)
{
tick_10ms=0;
juli = ceju();//获取距离
read_ds1302 ();//读取时间
mode_control ();//根据时间控制工作模式
}
}


int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//关闭LED外设
P2=0XA0;P0=0X00;P2=0X00;//关闭继电器蜂鸣器外设
P34=1;P35=1;P42=1;P44=1;//初始化按键引脚
init_ds1302 (0x06,0x59,0x00);//设置初始化时间
//write_password (6,5,4,3,2,1);//写初始密码到24c02
read_password();//读取24c02密码更新到数组
Timer1Init();//任务调度定时器初始化
while(1)
{	
key_value=read_keyboard();//读取键值
task_schedule ();//任务调度
control_center ();//系统中控（自动门和密码门逻辑实现）
}
	
}

void server () interrupt 3
{
static unsigned int tick_5s,tick_3s;//tick_5s,tick_3s;
tick_10ms++;//十秒任务调度标志位
if(door_5s==1){tick_5s++;if(tick_5s==5000){tick_5s=0;door_5s=0;relay=0;}}//继电器五秒延时
if(buzzer_3s==1){tick_3s++;if(tick_3s==3000){tick_3s=0;buzzer_3s=0;buzzer=0;}}//蜂鸣器三秒延时
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//数码管扫描标志位
smg(tick_8ms,SMG[tick_8ms]);//数码管更新	
hardware_control ();//硬件状态更新	（蜂鸣器、继电器）
}
