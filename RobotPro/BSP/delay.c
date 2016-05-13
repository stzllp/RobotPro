#include "delay.h"
#include "stm32f2xx.h"

void SysTick_DelayConfig(void)
{
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		//时钟源为系统时钟72MHz
	SysTick->LOAD = 120-1;									//重载值为72-1，每1us溢出一次
}

void delay_ms(uint32_t n)
{
	n *= 1000;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;			//使能SysTick，开始计数
	while(n--){
		while((SysTick->CTRL&0X010000) == 0);			//等待COUNTFLAG标志位置1
	}
	SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);		//失能SysTick，停止计数
}

void delay_us(uint32_t n)
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(n--){
		while((SysTick->CTRL&0X010000) == 0);
	}
	SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}
