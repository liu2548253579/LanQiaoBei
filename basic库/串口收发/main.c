#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include <STDIO.H>
int num;

/*若要实现printf重定向*/


//发送一个字节数据
//void sendByte(unsigned char dat)
//{
//   SBUF = dat;     //写入发送缓冲寄存器
//   while(!TI);    //等待发送完成，TI发送溢出标志位 置1
//   TI = 0;      //对溢出标志位清零	
//}

//重写printf调用的putchar函数，重定向到串口输出
//需要引入头文件<stdio.h>
//输出重定向到串口

char putchar(char c){
   SBUF = c;     //写入发送缓冲寄存器
   while(!TI);    //等待发送完成，TI发送溢出标志位 置1
   TI = 0;      //对溢出标志位清零	
	return c;  //返回给函数的调用者printf
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


void Delay100ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 144;
	k = 71;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


int main ()
{
P2=0X80;P0=0XFF;P2=0X00;//关闭LED
P2=0XA0;P0=0X00;P2=0X00;//关闭蜂鸣器、继电器
UartInit();//串口初始化
while(1)
{
printf("OK\r\n");
Delay100ms();
}
}

