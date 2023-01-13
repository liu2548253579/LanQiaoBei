#ifndef __DS1302_H
#define __DS1302_H

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte( unsigned char address );
//下面的需要自行添加（来自陈建伟学长的代码）
void wwrite_ds1302(unsigned char* p);
void rread_ds1302(unsigned char* p);
#endif
