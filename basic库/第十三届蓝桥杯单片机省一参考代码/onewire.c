#include "onewire.h"

#include "REG52.h"

sbit DQ = P1^4;  

//�������ڲ���ʱ����
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--)
	{
	for(i=0;i<12;i++);
	}
}

//������д����
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//�����߶�����
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20��ʼ��
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

float read_temp (void)
{
unsigned char low,high;
unsigned int tem;
float temp;
init_ds18b20();
Write_DS18B20(0XCC);
Write_DS18B20(0X44);	
Delay_OneWire(200); 

init_ds18b20();
Write_DS18B20(0XCC);
Write_DS18B20(0XBE);	
Delay_OneWire(200);
low=Read_DS18B20();
high=Read_DS18B20();	
	
high&=0X0F;
tem=(high<<8)+low;
temp=tem*0.0625;
return temp;
}