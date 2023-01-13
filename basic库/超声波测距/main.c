#include "stc15f2k60s2.h"


#define TX P10 
#define RX P11

unsigned char tick_8ms,tick_20ms,SMG[8];//8msʱ���־ 20msʱ���־ �����ˢ������
unsigned int juli=999;//������ 

unsigned char code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��

void Timer0Init(void)		//12΢��@12.000MHz
 {
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xF4;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}



unsigned int ceju(void)//�����������
{
	unsigned char a=10;//��������-����
	unsigned int juli=0;
	Timer0Init();
	TX=0;//�رշ�������
	
	while(a--)//����ʮ������
	{
		while(!TF0);//�ȴ����	
		TX=1;//��ʼ��������
		TF0=0;//���������־λ
	}
		TR0=0;//�رն�ʱ��0
	
	  TH0=0;//��ʱ��0��λ����
  	TL0=0;//��ʱ��0��λ����
	
	  TR0=1;//�򿪶�ʱ��0��ʼ��ʱ
	
	while(RX&&!TF0);//��RX���յ��������TF0���ʱ����
	if(TF0==1){juli=999;}//TF0���������Ϊ�����Ϊ999cm
	else{juli=((TH0<<8)+TL0)*0.017;}//���ݾ���ʱ�乫ʽ����ʵ�ʾ���
	
	TR0=0;//�رն�ʱ��0		
	return juli;
}



void smg (unsigned char duan,unsigned char wei)//�����ˢ�µ�Ԫ
{
P0=0XFF;
P2=0XC0;P0=T_COM[duan];P2=0X00;
P2=0XE0;P0=~t_display[wei];P2=0X00;
}

void Timer1Init(void)		//1����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x20;		//���ö�ʱ��ֵ
	TH1 = 0xD1;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
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
P2=0X80;P0=0XFF;P2=0X00;//�ر�LED����
P2=0XA0;P0=0X00;P2=0X00;//�رռ̵�������������

Timer1Init();//������ȶ�ʱ����ʼ��
while(1)
{	
Task();
}
	
}

void server () interrupt 3
{
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//�����ɨ���־λ
tick_20ms++;//�������20ms��һ�ξ���

smg(tick_8ms,SMG[tick_8ms]);//����ܸ���	
}
