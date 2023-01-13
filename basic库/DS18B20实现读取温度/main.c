#include "STC15F2K60S2.H"
#include "ONEWIRE.H"

char tik8ms; //ʱ���־λ���а˸�״̬ÿ�˺������һ��
char smg[8];// �����洢ÿ���������ʾ�Ķ�����������ʾʲô�ο�����t_display��

unsigned int wendu; //�洢�¶���ֵ

//���������������ü�����Դ�STC��ISP����л�ȡ�����ϽǱ����� -> STC�����壨������4ʾ������-> H595��������� ��> C��

/*************  ���س�������    **************/
char t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

char T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //λ��


//��ʱ�����ÿ��Դ�STC��ISP����еĶ�ʱ���������л�� ��1ms 16λ�Զ����� 1Tָ���
//ע�����������Ҫ������ӣ�	ET1 = 1; EA = 1;��
void Timer1Init(void)		//1����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x20;		//���ö�ʱ��ֵ
	TH1 = 0xD1;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;                           //�򿪶�ʱ��һ����Ҫ������ӣ�
	EA = 1;                            //���жϿ��أ���Ҫ������ӣ�
}

//�������ʾ��Ԫ
void SMG(char wei,char number)
{
P2=0XC0;P0=T_COM[wei];P2=0X00; //��ѡ 
P2=0XE0;P0=~t_display[number];P2=0X00;//λѡ��ע��STC�ṩ���ǹ�����ʹ��ʱע��ȡ����
P0=0X00; //������Ӱ�������
}

int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//�ر�LED
P2=0XA0;P0=0X00;P2=0X00;//�رռ̵����ͷ�����
Timer1Init();//��ʱ��һ��ʼ��
while(1)
{

smg[0]=wendu/1000;
smg[1]=wendu%1000/100+32;
smg[2]=wendu%100/10;
smg[3]=wendu%10;
	
smg[4]=16;smg[5]=16;smg[6]=16;smg[7]=16;
wendu=(unsigned int)(read_temp()*100); //��ȡ�¶�
}
}

void server ()interrupt 3  //��ʱ��1���������ֿ����⵫�Ǻ���һ��Ҫ�� interrupt 3���������õ�1ms����ÿһ����ͻ����˷�������
{
tik8ms=tik8ms+1;   //����ÿһ��������һ
if(tik8ms==8)tik8ms=0; //�޶�����0-7ѭ��
SMG(tik8ms,smg[tik8ms]); //���ˢ�������	
}
