#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include "reg52.h"

sbit DQ = P1^4;  

unsigned char rd_temperature(void);  

#endif
