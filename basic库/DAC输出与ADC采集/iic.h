/*
 * @Author: error: git config user.name && git config user.email & please set dead value or install git
 * @Date: 2022-02-24 16:12:39
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2022-12-26 19:27:51
 * @FilePath: \MP3d:\LEARNINGRESOURCES\蓝桥杯\蓝桥杯库\basic库\DAC输出与ADC采集\iic.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _IIC_H
#define _IIC_H

void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void); 
//以下代码需要自行添加
void dac(unsigned char dat);
unsigned char adc(unsigned char addr);
#endif