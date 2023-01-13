#include <STC15F2K60S2.H>
#include <ONEWIRE.H>
#include <INTRINS.H>
#include <DS1302.H>

bit controlmode,L1,L2,L3;//ģʽ��־��LED��־��
bit tick5s_relay_start,tick5s_led_start;//�̵���5s��ʱ��־λ��LED5s��ʱ��־λ�������ȴ���־λ
bit relay_state,read_line,time_key;//�̵���״̬��־����ȡ�����б�־��ʱ����ʾ�л���־λ

unsigned char smg_display_mode;

unsigned char tick_8ms,SMG[8],tick_30ms,tick_20ms,tick_100ms,tick_led_100ms;//ʱ���־λ�������

unsigned char temp_set=23;//��ʼֵ����
unsigned char shi,fen,miao;//ʱ���ȡ
unsigned int real_temp;//�¶ȶ�ȡ

unsigned int tick_relay_5s,tick_led_5s;//����256ms��ʱ���־λ

unsigned char code write_ds1302[8]={0X80,0X82,0X84,0X86,0X88,0X8A,0X8C,0X8E};
unsigned char code read_ds1302[8]={0X81,0X83,0X85,0X87,0X89,0X8B,0X8D,0X8F};


unsigned char code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��


void TimerInit(void)		//1����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x20;		//���ö�ʱ��ֵ
	TH1 = 0xD1;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1; 
  EA = 1; 	
}

void init_ds1302 (unsigned char shi_set,unsigned char fen_set,unsigned char miao_set)//��ʼ��ʱ��
{
Write_Ds1302_Byte(0X8E,0X00);	//�ر�д����
Write_Ds1302_Byte(0X84,shi_set);
Write_Ds1302_Byte(0X82,fen_set);
Write_Ds1302_Byte(0X80,miao_set);	
Write_Ds1302_Byte(0X8E,0X80);	//��д����
shi=Read_Ds1302_Byte(0X85);
fen=Read_Ds1302_Byte(0X83);
miao=Read_Ds1302_Byte(0X81);
}

void U1_display (void)//�¶���ʾ����
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


void U2_display (bit mode)//ʱ����ʾ����
{
if(mode==0)//��ʾʱ��
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

if(mode==1)//��ʾ����
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

void U3_display (void)//�¶����ý���
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

void led_set (void)//led����
{
P0=0XFF;
P2=0X80;
if(L1==1){P00=0;}if(L1==0){P00=1;}
if(L2==1){P01=0;}if(L2==0){P01=1;}	
if(L3==1){P02=0;}if(L3==0){P02=1;}
P03=1;P04=1;P05=1;P06=1;P07=1;
P2=0X00;
}

void smg (unsigned char wei,unsigned char duan)//�����ˢ����С��Ԫ
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

void keyboard (void)//���̶�ȡ�����
{
if(read_line==0)//��ȡ������
{
P32=0;P33=1;P34=1;P35=1;
	
if(P34==0)//S17����ʱ���л���
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

if(P35==0)//S13ģʽ�л�
{	
while(!P35){EA=1;};
P2=0XA0;P0=0X00;P2=0X00;relay_state=0;//�����̵���ԭ״̬
P2=0X80;P0=0XFF;P2=0X00;//����LEDԭ״̬
controlmode=~controlmode;//�л�����ģʽ
}

}

if(read_line==1)//��ȡ������
{
P32=1;P33=0;P34=1;P35=1;

if(P34==0)//S16��
{
 while(!P34){EA=1;};
if(smg_display_mode==2){temp_set=temp_set+1;}
if(temp_set>=99){temp_set=99;}
}
if(P35==0)//S12��ʾ�л�
{
 while(!P35){EA=1;};
smg_display_mode++;
if(smg_display_mode>=3){smg_display_mode=0;}
}

}

}


void task_schedule (void)
{
	if(tick_30ms>=30)//30ms��ȡһ���¶�
	{
	tick_30ms=0;
  real_temp=read_temp()*10;
	}
	
if(tick_8ms==0) //8ms��ȡһ��ʱ��
{
shi=Read_Ds1302_Byte(0X85);
fen=Read_Ds1302_Byte(0X83);
miao=Read_Ds1302_Byte(0X81);	
}	


}

int main ()
{
	P2=0X80;P0=0XFF;P2=0X00;//�ر�LED
	P2=0XA0;P0=0X00;P2=0X00;//�رշ������̵���
	init_ds1302 (0X23,0X59,0X55);//��ʼ��ʱ��
	TimerInit();//��ʼ����ʱ��
while (1)
{
task_schedule ();//�������
keyboard ();//��ȡ����
}
}





void control_mode (void)//ģʽ����
{
if(controlmode==0)//�¶ȿ���ģʽ
{
L2=1;//�¶�ģʽLED����
if(real_temp>temp_set*10){P2=0XA0;P0=0X00;P04=1;P2=0X00;relay_state=1;}else{P2=0XA0;P0=0X00;P2=0X00;relay_state=0;}//�¶ȼ̵�������
}	
if(controlmode==1)//ʱ�����ģʽ
{
L2=0;//ʱ��ģʽLED����	
if(fen==0X00&&miao==0X00){tick5s_relay_start=1;P2=0XA0;P0=0X00;P04=1;P2=0X00;relay_state=1;}//ʱ��̵�������	
if(fen==0X00&&miao==0X00){tick5s_led_start=1;L1=1;}//ʱ��led����	
}	
}

void display_mode (void)
{
if(smg_display_mode==0){U1_display();}//�¶���ʾģʽ
if(smg_display_mode==1){U2_display(0);}//ʱ����ʾģʽ1
if(smg_display_mode==1&&time_key==1){U2_display(1);}//ʱ����ʾģʽ2
if(smg_display_mode==2){U3_display();}//�¶��趨ģʽ
}




void server () interrupt 3
{
tick_8ms++;if(tick_8ms==8){tick_8ms=0;}//8ms��ʱ��־
smg(tick_8ms,SMG[tick_8ms]);//����ܶ�̬ˢ��
tick_20ms++;if(tick_20ms==20){tick_20ms=0;read_line=~read_line;}//20ms��ʱ��־ �л���ȡ������
tick_30ms++;//30ms��ʱ��־.

if(relay_state==1){tick_100ms++;if(tick_100ms==100){tick_100ms=0;L3=~L3;}}else{L3=0;}//L3״̬����
if(tick5s_relay_start==1){tick_relay_5s++;if(tick_relay_5s==5000){tick_relay_5s=0;tick5s_relay_start=0;P2=0XA0;P0=0X00;P2=0X00;relay_state=0;}}//�̵������ƣ�5s�رգ�
if(tick5s_led_start==1){tick_led_5s++;if(tick_led_5s==5000){tick_led_5s=0;tick5s_led_start=0;L1=0;}}//L1���ƣ�5s�رգ�

led_set ();//led�ܿ���
control_mode ();//�¶�ʱ��ģʽ����
display_mode ();//�������ʾģʽ����

}