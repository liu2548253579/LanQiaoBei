#include "STC15F2K60S2.H"
#include "ONEWIRE.H"

char tik8ms; //时间标志位，有八个状态每八毫秒更新一次
char smg[8];// 用来存储每个数码管显示的东西（具体显示什么参考数组t_display）

unsigned int wendu; //存储温度数值

//下面两个变量不用记忆可以从STC—ISP软件中获取（右上角边栏的 -> STC开发板（试验箱4示例程序）-> H595驱动数码管 —> C）

/*************  本地常量声明    **************/
char t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

char T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //位码


//定时器配置可以从STC—ISP软件中的定时器计算器中获得 （1ms 16位自动重载 1T指令集）
//注意最后两行需要自行添加（	ET1 = 1; EA = 1;）
void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;                           //打开定时器一（需要自行添加）
	EA = 1;                            //打开中断开关（需要自行添加）
}

//数码管显示单元
void SMG(char wei,char number)
{
P2=0XC0;P0=T_COM[wei];P2=0X00; //段选 
P2=0XE0;P0=~t_display[number];P2=0X00;//位选（注意STC提供的是共阴码使用时注意取反）
P0=0X00; //消除鬼影建议加上
}

int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//关闭LED
P2=0XA0;P0=0X00;P2=0X00;//关闭继电器和蜂鸣器
Timer1Init();//定时器一初始化
while(1)
{

smg[0]=wendu/1000;
smg[1]=wendu%1000/100+32;
smg[2]=wendu%100/10;
smg[3]=wendu%10;
	
smg[4]=16;smg[5]=16;smg[6]=16;smg[7]=16;
wendu=(unsigned int)(read_temp()*100); //读取温度
}
}

void server ()interrupt 3  //定时器1服务函数名字可随意但是后面一定要加 interrupt 3（由于配置的1ms，即每一毫秒就会进入此服务函数）
{
tik8ms=tik8ms+1;   //让其每一毫秒自增一
if(tik8ms==8)tik8ms=0; //限定其在0-7循环
SMG(tik8ms,smg[tik8ms]); //逐个刷新数码管	
}
