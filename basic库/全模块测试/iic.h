#ifndef _IIC_H
#define _IIC_H

void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void);

//以下为自写函数申明
void write_24c02 (unsigned char addr,unsigned char dat);
unsigned char read_24c02 (unsigned char addr);
void dac (unsigned char dat);
unsigned char adc (unsigned char addr);

#endif
