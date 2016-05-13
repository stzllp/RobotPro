#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include <stdint.h>

void SysTick_DelayConfig(void);
void delay_ms(uint32_t n);
void delay_us(uint32_t n);

#endif
