#include "delay.h"
#include "stm32f2xx.h"

void SysTick_DelayConfig(void)
{
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		//ʱ��ԴΪϵͳʱ��72MHz
	SysTick->LOAD = 120-1;									//����ֵΪ72-1��ÿ1us���һ��
}

void delay_ms(uint32_t n)
{
	n *= 1000;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;			//ʹ��SysTick����ʼ����
	while(n--){
		while((SysTick->CTRL&0X010000) == 0);			//�ȴ�COUNTFLAG��־λ��1
	}
	SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);		//ʧ��SysTick��ֹͣ����
}

void delay_us(uint32_t n)
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(n--){
		while((SysTick->CTRL&0X010000) == 0);
	}
	SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}
