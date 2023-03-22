#include "stc15f2k60s2.h"

unsigned char key_value=99;//��ֵ 
unsigned char tick_8ms,tick_2ms,SMG[8];//8msʱ���־ �����ˢ������ 

unsigned char code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��


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

void key_task (void)//��������ִ�е�����
{

}



//������ȡ����Ҫ�����Ŷ�ʱ����ڰ�������ʱ���ظ�ִ��key_task()��
unsigned char  read_keyboard (void)
{
	bit trig=0;//����������־λ
	unsigned char key_val;//��ֵ�ݴ�λ
	trig=0;//������־λ���㣨���Բ��ӣ�
	P44=0;P42=1;P35=1;P34=1;//ɨ���һ��
		if(P30==0){while(!P30){key_task();}trig=1;key_val=1;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=5;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=9;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=13;}
	P44=1;P42=0;P35=1;P34=1;//ɨ��ڶ���
		if(P30==0){while(!P30){key_task();}trig=1;key_val=2;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=6;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=10;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=14;}
	P44=1;P42=1;P35=0;P34=1;//ɨ�������
		if(P30==0){while(!P30){key_task();}trig=1;key_val=3;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=7;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=11;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=15;}
	P44=1;P42=1;P35=1;P34=0;//ɨ�������
		if(P30==0){while(!P30){key_task();}trig=1;key_val=4;}
		if(P31==0){while(!P31){key_task();}trig=1;key_val=8;}	
		if(P32==0){while(!P32){key_task();}trig=1;key_val=12;}	
		if(P33==0){while(!P33){key_task();}trig=1;key_val=16;}

		if(!trig){key_val=99;}//���ް����������ֵΪ99
		return key_val;//���¼�ֵ
}


void task_schedule (void)
{
	if(tick_2ms>=2)
	{
		tick_2ms=0;
		key_value=read_keyboard();//��ȡ��ֵ
	}
}


int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//�ر�LED����
P2=0XA0;P0=0X00;P2=0X00;//�رռ̵�������������
P34=1;P35=1;P42=1;P44=1;//��ʼ����������
Timer1Init();//������ȶ�ʱ����ʼ��
	while(1)
	{	
		task_schedule();
				key_value=read_keyboard();//��ȡ��ֵ
		if(key_value!=99){SMG[0]=key_value;}	//���������ı��ֵ
	}
		
}

void server () interrupt 3
{	
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//�����ɨ���־λ
smg(tick_8ms,SMG[tick_8ms]);//����ܸ���	
tick_2ms++;
}
