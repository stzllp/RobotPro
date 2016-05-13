#include "util.h"
//#include "printf.h"

void printClockFreq(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	SystemCoreClockUpdate();
	RCC_GetClocksFreq(&rcc_clocks);
//	printf("RCC_Clocks.SYSCLK_Frequency = %d\r\n", rcc_clocks.SYSCLK_Frequency);
//	printf("RCC_Clocks.HCLK_Frequency = %d\r\n", rcc_clocks.HCLK_Frequency);
//	printf("RCC_Clocks.PCLK1_Frequency = %d\r\n", rcc_clocks.PCLK1_Frequency);
//	printf("RCC_Clocks.PCLK2_Frequency = %d\r\n", rcc_clocks.PCLK2_Frequency);
}

//priority: 0~15, 越小优先级越高
void NVIC_Config(uint8_t irq, uint8_t priority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	uint8_t pre, sub;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	pre = priority/4;
	sub = priority%4;
	NVIC_InitStructure.NVIC_IRQChannel = irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pre;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

uint8_t checkSum(uint8_t *buf, uint32_t len)
{
	uint32_t i;
	uint16_t sum = 0;
	for(i=0; i<len; i++){
		sum += buf[i];
	}
	return ~(uint8_t)sum;
}


