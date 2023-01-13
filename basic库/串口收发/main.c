#include <STC15F2K60S2.H>
#include <INTRINS.H>
#include <STDIO.H>
int num;

/*��Ҫʵ��printf�ض���*/


//����һ���ֽ�����
//void sendByte(unsigned char dat)
//{
//   SBUF = dat;     //д�뷢�ͻ���Ĵ���
//   while(!TI);    //�ȴ�������ɣ�TI���������־λ ��1
//   TI = 0;      //�������־λ����	
//}

//��дprintf���õ�putchar�������ض��򵽴������
//��Ҫ����ͷ�ļ�<stdio.h>
//����ض��򵽴���

char putchar(char c){
   SBUF = c;     //д�뷢�ͻ���Ĵ���
   while(!TI);    //�ȴ�������ɣ�TI���������־λ ��1
   TI = 0;      //�������־λ����	
	return c;  //���ظ������ĵ�����printf
}




void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xC7;		//�趨��ʱ��ֵ
	TH1 = 0xFE;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
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
P2=0X80;P0=0XFF;P2=0X00;//�ر�LED
P2=0XA0;P0=0X00;P2=0X00;//�رշ��������̵���
UartInit();//���ڳ�ʼ��
while(1)
{
printf("OK\r\n");
Delay100ms();
}
}

