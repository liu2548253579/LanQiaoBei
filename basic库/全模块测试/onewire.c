#include "onewire.h"

#include "reg52.h"

sbit DQ = P1^4;  

//
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--)
	{
	for(i=0;i<12;i++);
	}
}

//
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

//
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

//
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


float _read_ds18b20 (void)
{
unsigned char low,high;
unsigned int temp;
float tem;
init_ds18b20();
Write_DS18B20(0xcc);
Write_DS18B20(0x44);
Delay_OneWire(200);  	
	
init_ds18b20();
Write_DS18B20(0xcc);
Write_DS18B20(0xbe);
Delay_OneWire(200); 	

low=Read_DS18B20();
high=Read_DS18B20();	
	
high&=0x0f;
temp=(high<<8)+low;
tem=temp*0.0625;
	
return tem;	
}



unsigned int ceju (void)
{
unsigned char times;
unsigned int juli;
Timer0Init();
TX=0;
while(times--)
{
while(!TF0);
TX=1;
TF0=0;
}
TR0=0;TH0=0;TL0=0;TR0=1;
while(RX&&!TF0);
if(TF0==1){juli=999;}
else{juli=((TH0<<8)+TL0)*0.017;}
TR0=0;
return juli;
}


char putchar (char c)
{
SBUF=c;
while(!TI);
TI=0;
return c;
}