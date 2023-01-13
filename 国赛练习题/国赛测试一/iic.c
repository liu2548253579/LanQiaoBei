#include "iic.h"
#include "reg52.h"
#include "intrins.h"

sbit SDA = P2^1;
sbit SCL = P2^0;


#define DELAY_TIME 5

//I2C总线内部延时函数
void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}

//I2C总线启动信号
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//I2C总线停止信号
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

////发送应答或非应答信号
//void IIC_SendAck(bit ackbit)
//{
//    SCL = 0;
//    SDA = ackbit;  					
//    IIC_Delay(DELAY_TIME);
//    SCL = 1;
//    IIC_Delay(DELAY_TIME);
//    SCL = 0; 
//    SDA = 1;
//    IIC_Delay(DELAY_TIME);
//}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//I2C总线发送一个字节数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//I2C总线接收一个字节数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}


void write_24c02 (unsigned char addr,unsigned char dat)
{
IIC_Start();
IIC_SendByte(0XA0);
IIC_WaitAck();
IIC_SendByte(addr);
IIC_WaitAck();
IIC_SendByte(dat);	
IIC_WaitAck();
IIC_Stop();
}


unsigned char read_24c02(unsigned char addr)
{
  unsigned char temp;
	EA=0;
	IIC_Start();
	IIC_SendByte(0XA0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	
	IIC_Start();
	IIC_SendByte(0xA1);
	IIC_WaitAck();
	temp=IIC_RecByte();
	IIC_WaitAck();
	IIC_Stop();
	EA=1;
	return temp;
}	
	