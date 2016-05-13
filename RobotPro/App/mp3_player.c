#include "stm32f2xx.h"
#include <string.h>
#include "ff.h"
#include "mp3dec.h"
#include "bsp_dma.h"
#include "bsp_dac.h"
#include "timer.h"


// 文件缓冲
#define IN_BUFFERSIZE 1024*2
#define MINFRAMESIZE 1024
u8 in_buffer[IN_BUFFERSIZE];
// 使用双缓冲输出
#define OUT_BUFFERSIZE 2400
short out_buffer[2][OUT_BUFFERSIZE];

#define LEDON GPIO_SetBits(GPIOA, GPIO_Pin_8);
#define LEDOFF GPIO_ResetBits(GPIOA, GPIO_Pin_8);

void MP3_PlayerInit(void)
{
	BSP_DMA_Init();
	BSP_DAC_Init();
	TIM6_Init(44100); // 默认44.1khz采样率
}

static void  delay_ms (u32  dly)
{
    volatile  u32  i,j;

    for (i = 0; i < dly; i++) 
    {
        for (j = 0; j < 1000; j++) 
        {				
        }
    }
}


/*
 * 将解码出来的数据用DMA发到DAC
 * 用DMA2 CH3（DAC1）
 * TIM6会触发DAC1，然后DAC1会给出DMA请求
 * 如果是双声道的，解码出来的PCM数据以L1,R1,L2,R2,L3,R3...来排列
 * DMA将32位数据搬运到DAC_DHR12LD寄存器里面，一次设置两个DAC的值
 */
void SendToDAC(int samprate, int nchannel, void*data, int size)
{
	// 如果开了DMA的话，等待DMA传输完成
	while((DMA1_Stream5->CR & DMA_SxCR_EN) && DMA1_Stream5->NDTR);
    DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);//Streamx transfer complete flag
	
	SetBitRate(samprate);
	
	DMA1_Stream5->CR &= ~DMA_SxCR_EN;
	if(nchannel == 1)
	{
		/* 单声道 */
		DMA1_Stream5->CR &= ~DMA_SxCR_MSIZE;
		DMA1_Stream5->CR |= DMA_MemoryDataSize_HalfWord;
	}
	else
	{
		/* 双声道 */
		DMA1_Stream5->CR &= ~DMA_SxCR_MSIZE;
		DMA1_Stream5->CR |= DMA_MemoryDataSize_Word;//一次传输32bi覆盖DAC 通道1 2 
		size >>= 1;
	}
	DMA1_Stream5->M0AR = (u32)data;
	DMA1_Stream5->NDTR = size;
	DMA1_Stream5->CR |= DMA_SxCR_EN;
}

void PlayMP3(char filename[])
{
	FIL file;
	
	if(f_open(&file, filename, FA_READ) == FR_OK)
	{
		HMP3Decoder decoder = MP3InitDecoder();
		if(decoder != 0)
		{
			int bytes_left = 0;
			UINT rwnum = 0;
			u8 *in_ptr = in_buffer;
			int out_switch = 0;
			
			// 循环解码整个文件
			while(
				f_read(&file, in_buffer + bytes_left, sizeof(in_buffer) - bytes_left, &rwnum) == FR_OK
					&&
				file.fptr < file.fsize
				 )
			{
				int sync_offset;
				in_ptr = in_buffer;
				bytes_left += rwnum;
				// 逐帧解码
				while((sync_offset = MP3FindSyncWord(in_ptr, bytes_left)) >= 0)
				{
					in_ptr += sync_offset;
					bytes_left -= sync_offset;
					if(bytes_left < MINFRAMESIZE)
					{
						// 剩余数据不够一帧
						memcpy(in_buffer, in_ptr, bytes_left);
						// 返回上一层循环，补充数据
						break;
					}
					// 解码
					if(MP3Decode(decoder, &in_ptr, &bytes_left, out_buffer[out_switch], 0) == ERR_MP3_NONE)
					{
						MP3FrameInfo frameinfo;
						int i;
						uint8_t temp[2];
						MP3GetLastFrameInfo(decoder, &frameinfo);
						for(i = frameinfo.outputSamps-1; i >= 0; i--)
						{
							// PCM有符号数，抬高0x8000成无符号数让DAC输出
							out_buffer[out_switch][i] ^= 0x8000;
						}
						SendToDAC(frameinfo.samprate, frameinfo.nChans, out_buffer[out_switch], frameinfo.outputSamps);
						out_switch ^= 1;
					}
					else
					{
						// 跳过这一帧
						in_ptr ++;
						bytes_left --;
					}
				}
				if(sync_offset < 0)
				{
					bytes_left = 0;
				}
			}
		}
		MP3FreeDecoder(decoder);
	}
	f_close(&file);
}

void PowerSwitchInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;               
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

#define PowerOn() GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define PowerOff() GPIO_ResetBits(GPIOC, GPIO_Pin_7)
