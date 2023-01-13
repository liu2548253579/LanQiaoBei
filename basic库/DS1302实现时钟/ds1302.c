/*
  ����˵��: DS1302��������
  �������: Keil uVision 4.10
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨ 8051��12MHz
  ��    ��: 2011-8-9
*/

#include <reg52.h>
#include <intrins.h>

sbit SCK=P1^7;
sbit SDA=P2^3;
sbit RST = P1^3;   // DS1302��λ

void Write_Ds1302(unsigned  char temp)
{
    unsigned char i;
    for (i=0;i<8;i++)
    {
        SCK=0;
        SDA=temp&0x01;
        temp>>=1;
        SCK=1;
    }
}

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )
{
    RST=0;	_nop_();
    SCK=0;	_nop_();
    RST=1;      _nop_();
    Write_Ds1302(address);
    Write_Ds1302(dat);
    RST=0;
}

unsigned char Read_Ds1302_Byte ( unsigned char address )
{
    unsigned char i,temp=0x00;
    RST=0;	_nop_();
    SCK=0;	_nop_();
    RST=1;	_nop_();
    Write_Ds1302(address);
    for (i=0;i<8;i++)
    {
        SCK=0;
        temp>>=1;
        if(SDA)
            temp|=0x80;
        SCK=1;
    }
    RST=0;	_nop_();
    SCK=0;	_nop_();
    SCK=1;	_nop_();
    SDA=0;	_nop_();
    SDA=1;	_nop_();
    return (temp);
}

//�������Ҫ������ӣ����Գ½�ΰѧ���Ĵ��룩
void wwrite_ds1302(unsigned char* p)
{
	Write_Ds1302_Byte(0x8e,0x00);
	Write_Ds1302_Byte(0x80,((p[2]/10)<<4)+p[2]%10);
	Write_Ds1302_Byte(0x82,((p[1]/10)<<4)+p[1]%10);
	Write_Ds1302_Byte(0x84,((p[0]/10)<<4)+p[0]%10);
	Write_Ds1302_Byte(0x8e,0x80);
}
void rread_ds1302(unsigned char* p)
{
	unsigned char temp;
	temp=Read_Ds1302_Byte(0x81);
	p[4]=temp/16;
	p[5]=temp%16;
	temp=Read_Ds1302_Byte(0x83);
	p[2]=temp/16;
	p[3]=temp%16;
	temp=Read_Ds1302_Byte(0x85);
	p[0]=temp/16;
	p[1]=temp%16;
}