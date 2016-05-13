#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

typedef void(*TimerHandler)(void);

void TIM6_Init(int samprate);
void SetBitRate(int samprate);
void TIM7_Init(TimerHandler handler);
void TIM7_Start(uint16_t ms);
void TIM7_Stop(void);
void TIM2_Init(uint16_t ms, TimerHandler handler);
void TIM3_Init(uint16_t ms, TimerHandler handler);

#endif

