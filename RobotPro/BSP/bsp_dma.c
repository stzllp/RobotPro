#include "bsp_dma.h"
#include "stm32f2xx_dma.h"

void BSP_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	/* Enable DMA1's AHB1 interface clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	// DMA for DAC1
	DMA_InitStruct.DMA_Channel = DMA_Channel_7;
	DMA_InitStruct.DMA_BufferSize = 0;//0
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
	//DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	//DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	//DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	DMA_InitStruct.DMA_Memory0BaseAddr = 0;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; //32λ
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&DAC->DHR12LD;  // DUAL DAC 12-bit left aligned data holding register
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;// 32λ
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	DMA_DeInit(DMA1_Stream5);
	DMA_Init(DMA1_Stream5, &DMA_InitStruct);
	/* Enable DMA1 Stream5 */
	//DMA_Cmd(DMA1_Stream5, ENABLE);
}