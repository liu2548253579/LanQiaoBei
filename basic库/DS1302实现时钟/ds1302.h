#ifndef __DS1302_H
#define __DS1302_H

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte( unsigned char address );
//�������Ҫ������ӣ����Գ½�ΰѧ���Ĵ��룩
void wwrite_ds1302(unsigned char* p);
void rread_ds1302(unsigned char* p);
#endif
