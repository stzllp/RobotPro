#ifndef __UTIL_H__
#define __UTIL_H__

#include "stm32f2xx.h"

#define mymax(a, b)			(a>b? a:b)
#define mymin(a, b)			(a<b? a:b)
#define range(x, a, b)		(mymin(mymax(x, a), b))
#define exchange(a, b, tmp) (tmp=a, a=b, b=tmp)
#define myabs(x)			((x<0)? -x:x)

void printClockFreq(void);
void NVIC_Config(uint8_t irq, uint8_t priority);
uint8_t checkSum(uint8_t *buf, uint32_t len);

#endif
