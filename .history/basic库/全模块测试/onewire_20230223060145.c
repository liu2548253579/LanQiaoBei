/*
 * @Author: Frank 2548253579@qq.com
 * @Date: 2023-02-23 05:57:12
 * @LastEditors: Frank 2548253579@qq.com
 * @LastEditTime: 2023-02-23 06:01:40
 * @FilePath: \LanQiaoBei\basic库\全模块测试\onewire.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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


float read_temp (void)
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


