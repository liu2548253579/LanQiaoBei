#include<STC15F2K60S2.H>
#include<INTRINS.H>
#include<IIC.H>
unsigned char code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��
unsigned char tick_8ms,smg[8],content;

void Timer1Init(void)		//1����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x20;		//���ö�ʱ��ֵ
	TH1 = 0xD1;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;    //������ʱ��1
	EA  = 1;    //�������ж�
}

void SMG (unsigned char wei,unsigned char duan)
{
P2=0XC0;P0=T_COM[wei];P2=0X00;
P2=0XE0;P0=~t_display[duan];P2=0X00;
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





int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//�ر�led
P2=0XA0;P0=0X00;P2=0X00;//�رշ������رռ̵���
//write24c02(0x01,0x03);
Delay4ms();
content=read24c02(0x01);	
Timer1Init();
while(1)
{


smg[0]=content;
	
	
	
}
}

void server (void) interrupt 3
{
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}
SMG(tick_8ms,smg[tick_8ms]);
	
}
