#include <STC15F2K60S2.H>
#include <ONEWIRE.H>
#include <INTRINS.H>
#include <DS1302.H>

bit controlmode,L1,L2,L3;//模式标志、LED标志、
bit tick5s_relay_start,tick5s_led_start;//继电器5s定时标志位，LED5s定时标志位，按键等待标志位
bit relay_state,read_line,time_key;//继电器状态标志、读取键盘行标志、时间显示切换标志位

unsigned char smg_display_mode;

unsigned char tick_8ms,SMG[8],tick_30ms,tick_20ms,tick_100ms,tick_led_100ms;//时间标志位与数码管

unsigned char temp_set=23;//初始值设置
unsigned char shi,fen,miao;//时间读取
unsigned int real_temp;//温度读取

unsigned int tick_relay_5s,tick_led_5s;//超过256ms的时间标志位

unsigned char code write_ds1302[8]={0X80,0X82,0X84,0X86,0X88,0X8A,0X8C,0X8E};
unsigned char code read_ds1302[8]={0X81,0X83,0X85,0X87,0X89,0X8B,0X8D,0X8F};


unsigned char code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码


void TimerInit(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1; 
  EA = 1; 	
}

void init_ds1302 (unsigned char shi_set,unsigned char fen_set,unsigned char miao_set)//初始化时钟
{
Write_Ds1302_Byte(0X8E,0X00);	//关闭写保护
Write_Ds1302_Byte(0X84,shi_set);
Write_Ds1302_Byte(0X82,fen_set);
Write_Ds1302_Byte(0X80,miao_set);	
Write_Ds1302_Byte(0X8E,0X80);	//打开写保护
shi=Read_Ds1302_Byte(0X85);
fen=Read_Ds1302_Byte(0X83);
miao=Read_Ds1302_Byte(0X81);
}

void U1_display (void)//温度显示界面
{
SMG[0]=25;
SMG[1]=1;
SMG[2]=16;
SMG[3]=16;
SMG[4]=16;
SMG[5]=real_temp/100;
SMG[6]=real_temp/10%10+32;
SMG[7]=real_temp%10;
}


void U2_display (bit mode)//时间显示界面
{
if(mode==0)//显示时分
{	
SMG[0]=25;
SMG[1]=2;
SMG[2]=16;
SMG[3]=shi/16;
SMG[4]=shi%16;
SMG[5]=17;
SMG[6]=fen/16;
SMG[7]=fen%16;
}

if(mode==1)//显示分秒
{
SMG[0]=25;
SMG[1]=2;
SMG[2]=16;
SMG[3]=fen/16;
SMG[4]=fen%16;
SMG[5]=17;
SMG[6]=miao/16;
SMG[7]=miao%16;
}
}

void U3_display (void)//温度设置界面
{
SMG[0]=25;
SMG[1]=3;
SMG[2]=16;
SMG[3]=16;
SMG[4]=16;
SMG[5]=16;
SMG[6]=temp_set/10%10;
SMG[7]=temp_set%10;
}

void led_set (void)//led控制
{
P0=0XFF;
P2=0X80;
if(L1==1){P00=0;}if(L1==0){P00=1;}
if(L2==1){P01=0;}if(L2==0){P01=1;}	
if(L3==1){P02=0;}if(L3==0){P02=1;}
P03=1;P04=1;P05=1;P06=1;P07=1;
P2=0X00;
}

void smg (unsigned char wei,unsigned char duan)//数码管刷新最小单元
{
P0=0XFF;
P2=0XC0;P0=T_COM[wei];P2=0X00;
P2=0XE0;P0=~t_display[duan];P2=0X00;
}

void Delay10ms(void)		//@12.000MHz
{
	unsigned char i, j;

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}

void keyboard (void)//键盘读取与控制
{
if(read_line==0)//读取第三行
{
P32=0;P33=1;P34=1;P35=1;
	
if(P34==0)//S17减（时间切换）
{
if(smg_display_mode==1){time_key=1;}

	while(!P34)
		{
	Delay10ms();
	fen=Read_Ds1302_Byte(0X83);
miao=Read_Ds1302_Byte(0X81);
    };
		
if(smg_display_mode==2){temp_set=temp_set-1;}
if(temp_set<=10){temp_set=10;}
}
else
{
if(smg_display_mode==1){time_key=0;}
}

if(P35==0)//S13模式切换
{	
while(!P35){EA=1;};
P2=0XA0;P0=0X00;P2=0X00;relay_state=0;//消除继电器原状态
P2=0X80;P0=0XFF;P2=0X00;//消除LED原状态
controlmode=~controlmode;//切换工作模式
}

}

if(read_line==1)//读取第四行
{
P32=1;P33=0;P34=1;P35=1;

if(P34==0)//S16加
{
 while(!P34){EA=1;};
if(smg_display_mode==2){temp_set=temp_set+1;}
if(temp_set>=99){temp_set=99;}
}
if(P35==0)//S12显示切换
{
 while(!P35){EA=1;};
smg_display_mode++;
if(smg_display_mode>=3){smg_display_mode=0;}
}

}

}


void task_schedule (void)
{
	if(tick_30ms>=30)//30ms读取一次温度
	{
	tick_30ms=0;
  real_temp=read_temp()*10;
	}
	
if(tick_8ms==0) //8ms读取一次时间
{
shi=Read_Ds1302_Byte(0X85);
fen=Read_Ds1302_Byte(0X83);
miao=Read_Ds1302_Byte(0X81);	
}	


}

int main ()
{
	P2=0X80;P0=0XFF;P2=0X00;//关闭LED
	P2=0XA0;P0=0X00;P2=0X00;//关闭蜂鸣器继电器
	init_ds1302 (0X23,0X59,0X55);//初始化时钟
	TimerInit();//初始化定时器
while (1)
{
task_schedule ();//任务调度
keyboard ();//读取键盘
}
}





void control_mode (void)//模式控制
{
if(controlmode==0)//温度控制模式
{
L2=1;//温度模式LED控制
if(real_temp>temp_set*10){P2=0XA0;P0=0X00;P04=1;P2=0X00;relay_state=1;}else{P2=0XA0;P0=0X00;P2=0X00;relay_state=0;}//温度继电器控制
}	
if(controlmode==1)//时间控制模式
{
L2=0;//时间模式LED控制	
if(fen==0X00&&miao==0X00){tick5s_relay_start=1;P2=0XA0;P0=0X00;P04=1;P2=0X00;relay_state=1;}//时间继电器控制	
if(fen==0X00&&miao==0X00){tick5s_led_start=1;L1=1;}//时间led控制	
}	
}

void display_mode (void)
{
if(smg_display_mode==0){U1_display();}//温度显示模式
if(smg_display_mode==1){U2_display(0);}//时间显示模式1
if(smg_display_mode==1&&time_key==1){U2_display(1);}//时间显示模式2
if(smg_display_mode==2){U3_display();}//温度设定模式
}




void server () interrupt 3
{
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//8ms定时标志
smg(tick_8ms,SMG[tick_8ms]);//数码管动态刷新
tick_20ms++;if(tick_20ms==20){tick_20ms=0;read_line=~read_line;}//20ms定时标志 切换读取键盘行
tick_30ms++;//30ms定时标志.

if(relay_state==1){tick_100ms++;if(tick_100ms==100){tick_100ms=0;L3=~L3;}}else{L3=0;}//L3状态控制
if(tick5s_relay_start==1){tick_relay_5s++;if(tick_relay_5s==5000){tick_relay_5s=0;tick5s_relay_start=0;P2=0XA0;P0=0X00;P2=0X00;relay_state=0;}}//继电器控制（5s关闭）
if(tick5s_led_start==1){tick_led_5s++;if(tick_led_5s==5000){tick_led_5s=0;tick5s_led_start=0;L1=0;}}//L1控制（5s关闭）

led_set ();//led总控制
control_mode ();//温度时间模式控制
display_mode ();//数码管显示模式控制

}