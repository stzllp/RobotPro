#include "bsp_dac.h"
#include "stm32f2xx_dac.h"

static void dac_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void BSP_DAC_Init(void)
{
	DAC_InitTypeDef DAC_InitStructure;
	/* Enable DAC clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	dac_io_init();
	
	/*DAC channel 1 & 2 */
	/*都用TIM6作触发 */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=	DAC_LFSRUnmask_Bit0;  
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	
	DAC_DeInit();
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);
	//只开DAC1的DMA请求
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}