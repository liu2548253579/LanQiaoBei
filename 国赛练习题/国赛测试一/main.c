#include "stc15f2k60s2.h"
#include "iic.h"
#include "ds1302.h"
#include "intrins.h"

#define TX P10 
#define RX P11

bit relay,buzzer;//�̵��������������Ʊ�־λ
bit working_mode=1;//Ϊ�����Զ���Ϊһ��������
bit door_5s,buzzer_3s;//�������ͼ̵�������ʱ������־λ
unsigned int gao=654,di=321;//���ڴ洢����ĸ���λ�͵���λ
unsigned char display_mode,logic_mode,key_enter_flag,key_value,error_times;//��ʾģʽ �߼�ģʽ ���������־λ��λ���� ��ֵ �������
unsigned char tick_8ms,tick_10ms,SMG[8],shi,fen,miao;//8msʱ���־ 10msʱ���־ �����ˢ������ ʱ �� ��
unsigned int juli=999;//������ 
unsigned int password[6]={16,16,16,16,16,16},password_gao,password_di;//��λ���������ݴ�λ �ݴ����λ �ݴ����λ

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
	unsigned char a=10;
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

void hardware_control (void)//Ӳ������
{
P0=0X00;
P2=0XA0;
if(buzzer==1){P06=1;}
if(buzzer==0){P06=0;}

if(relay==1){P04=1;}
if(relay==0){P04=0;}
P2=0X00;
}


void init_ds1302 (unsigned char shi_set,unsigned char fen_set,unsigned char miao_set)//����ds1302ʱ�䲢���µ�ȫ�ֱ���
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

void read_ds1302 (void)//��ȡ������ds1302ʱ����Ϣ
{
	EA=0;
shi=Read_Ds1302_Byte(0x85);
fen=Read_Ds1302_Byte(0x83);
miao=Read_Ds1302_Byte(0x81);
	EA=1;
}

void display_ds1302 (void)//��ʾʱ��
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

void display_enter (void)//��ʾ������Ľ���
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

void display_config (void)//��ʾȷ�Ͼ��������
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

void display_new (void)//��ʾ�������������
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

void display_change (void)//����ģʽ����״̬�л�
{
if(display_mode==0){display_enter();}
if(display_mode==1){display_config();}
if(display_mode==2){display_new();}
}

void Delay4ms()		//@12.000MHz   4ms��ʱ��24c02��
{
	unsigned char i, j;

	i = 47;
	j = 174;
	do
	{
		while (--j);
	} while (--i);
}

unsigned char config_24c02_state (unsigned char mode)//���û��߶�ȡ24c02�����Ƿ񱻸�д��
{
if(mode==0){Delay4ms();write_24c02 (0x07,0);}
if(mode==1){Delay4ms();write_24c02 (0x07,7);}	
Delay4ms();	
return read_24c02(0x07);	
}


//������д��24c02
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


//�������24c02���µ�gao��di
void read_password (void)
{
unsigned char a1,a2,a3,a4,a5,a6;
if(config_24c02_state(2)==7)//������뱻��д�����ȡ
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


unsigned char read_keyboard (void)//���ض�ȡ���İ�����ֵ���ް��������򷵻�99
{
static unsigned char keyboard_tick;//��ѯ��־λ
unsigned char	key_val,key_trig=0;//��ֵ�ݴ棬������־

key_trig=0;	//���㴥��
	
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
if(P35==0){while(!P35){key_task();}key_val=10;key_trig=1;}//	����
if(P34==0){while(!P34){key_task();}key_val=11;key_trig=1;}	//��λ
}

if(keyboard_tick==3)
{
P30=1;P31=1;P32=1;P33=0;
if(P35==0){while(!P35){key_task();}key_val=12;key_trig=1;}//	ȷ��
if(P34==0){while(!P34){key_task();}key_val=13;key_trig=1;}	//�˳�
}
keyboard_tick=keyboard_tick+1;//��ѯ����

if(keyboard_tick==4){keyboard_tick=0;}


if(key_trig){return key_val;}//��������򷵻ؼ�ֵ
else{return 99;}//���δ�����򷵻ش�����99
}

void clean_password (void)//��������λ��������
{
password[0]=16,password[1]=16,password[2]=16,password[3]=16,password[4]=16,password[5]=16;
key_enter_flag=0;
}

void control_center (void)//ϵͳ�߼�����
{
if(working_mode==0)//�Զ���
{
display_ds1302 ();//��ʾʱ�����
if(juli<30){relay=1;door_5s=1;}//�����������30cm����
}

if(working_mode==1)//������
{
display_change();//��ʾ��ͬ����������ʾ
if(key_value==11){write_password (6,5,4,3,2,1);read_password();	config_24c02_state(0);}//��λ ��������Ϊ654321 ��ȡ���� ��24c02״̬��Ϊδ��д




if(logic_mode==0)//��������ģʽ
{
display_mode=0;//�л�����ģʽ��ʾΪ������ʾ
if(key_value<=9){password[key_enter_flag]=key_value;key_enter_flag=key_enter_flag+1;if(key_enter_flag>=5){key_enter_flag=5;}}//�����ֵС��9��Ϊ���֣��� ����Ӧ��λ����ֵ����Ӧ������ ������������λ��

if(key_value==12)//ȷ��
{
password_gao=password[0]*100+password[1]*10+password[2];//�������λ���븳ֵ���ݴ����λ
password_di=password[3]*100+password[4]*10+password[5];//�������λ���븳ֵ���ݴ����λ
if(password_gao==gao)//�ж������Ƿ���ȷ
{
if(password_di==di){clean_password();error_times=0;relay=1;door_5s=1;}//������� ������������� �򿪼̵��� ������ʱ5s
}

else{error_times++;clean_password();}//������� ���������һ �������
if(error_times>=3){error_times=0;buzzer=1;buzzer_3s=1;}//��������������� ���������� �򿪷����� ������ʱ3s
}

}

if(key_value==10)//����
{
clean_password();//�������
logic_mode=1;//�л�Ϊ��������ģʽ
}


if(logic_mode==1)//��������ģʽ
{
display_mode=1;//�л�����ģʽ��ʾΪ������ʾ
if(key_value==13){logic_mode=0;clean_password();}//�˳� �л�Ϊ��������ģʽ �������
else
{
if(key_value<=9){password[key_enter_flag]=key_value;key_enter_flag=key_enter_flag+1;if(key_enter_flag>=5){key_enter_flag=5;}}//�����ֵС��9��Ϊ���֣��� ����Ӧ��λ����ֵ����Ӧ������ ������������λ��
if(key_value==12)//ȷ��
{
password_gao=password[0]*100+password[1]*10+password[2];//�������λ���븳ֵ���ݴ����λ
password_di=password[3]*100+password[4]*10+password[5];//�������λ���븳ֵ���ݴ����λ
if(password_gao==gao)//�ж������Ƿ���ȷ
{
if(password_di==di){clean_password();logic_mode=2;error_times=0;key_value=99;}//������� �л�Ϊ����������ģʽ ������������� ����ֵ��ʱ����Ϊ99�������ȷ������
}

else{error_times++;clean_password();}//������� ���������һ �������
if(error_times>=3){error_times=0;buzzer=1;buzzer_3s=1;logic_mode=0;}//��������������� ���������� �򿪷����� ������ʱ3s
}
}
}

if(logic_mode==2)//����������
{
display_mode=2;//�л�����ģʽ��ʾΪ������������ʾ
if(key_value==13){logic_mode=0;clean_password();}//�˳�
else
{
if(key_value<=9){password[key_enter_flag]=key_value;key_enter_flag=key_enter_flag+1;if(key_enter_flag>=5){key_enter_flag=5;}}//�����ֵС��9��Ϊ���֣��� ����Ӧ��λ����ֵ����Ӧ������ ������������λ��
if(key_value==12)//ȷ��
{
write_password (password[0],password[1],password[2],password[3],password[4],password[5]);//����Ӧ�����������д���Ӧ��λ��
read_password();//�������뵽����
config_24c02_state(1);//���������д״̬Ϊ�����޸�
clean_password();//�������
logic_mode=0;	//�л�Ϊ��������ģʽ
}
}
}

}

}

void mode_control (void)//����ʱ���л�����ģʽ
{
if(shi>=0x07){if(shi<=0x21){working_mode=0;}}//��7�㵽21��59��59��Ϊ�Զ��ţ�
else{working_mode=1;}//������ʱ��Ϊ�����ţ�
}

void task_schedule (void)//��ʱ���������
{
if(tick_10ms>=10)
{
tick_10ms=0;
juli = ceju();//��ȡ����
read_ds1302 ();//��ȡʱ��
mode_control ();//����ʱ����ƹ���ģʽ
}
}


int main (void)
{
P2=0X80;P0=0XFF;P2=0X00;//�ر�LED����
P2=0XA0;P0=0X00;P2=0X00;//�رռ̵�������������
P34=1;P35=1;P42=1;P44=1;//��ʼ����������
init_ds1302 (0x06,0x59,0x00);//���ó�ʼ��ʱ��
//write_password (6,5,4,3,2,1);//д��ʼ���뵽24c02
read_password();//��ȡ24c02������µ�����
Timer1Init();//������ȶ�ʱ����ʼ��
while(1)
{	
key_value=read_keyboard();//��ȡ��ֵ
task_schedule ();//�������
control_center ();//ϵͳ�пأ��Զ��ź��������߼�ʵ�֣�
}
	
}

void server () interrupt 3
{
static unsigned int tick_5s,tick_3s;//tick_5s,tick_3s;
tick_10ms++;//ʮ��������ȱ�־λ
if(door_5s==1){tick_5s++;if(tick_5s==5000){tick_5s=0;door_5s=0;relay=0;}}//�̵���������ʱ
if(buzzer_3s==1){tick_3s++;if(tick_3s==3000){tick_3s=0;buzzer_3s=0;buzzer=0;}}//������������ʱ
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//�����ɨ���־λ
smg(tick_8ms,SMG[tick_8ms]);//����ܸ���	
hardware_control ();//Ӳ��״̬����	�����������̵�����
}
