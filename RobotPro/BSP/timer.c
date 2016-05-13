#include "timer.h"
#include "util.h"

TimerHandler TIM7_Handler;
TimerHandler TIM2_Handler;
TimerHandler TIM3_Handler;

void TIM6_Init(int samprate)
{
	TIM_TimeBaseInitTypeDef tim6;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	tim6.TIM_Period = 120000000L/samprate;    //44.1khz采样率，2.26us溢出时间
	tim6.TIM_Prescaler = 0;
	tim6.TIM_ClockDivision = TIM_CKD_DIV1;     
	tim6.TIM_CounterMode = TIM_CounterMode_Up; 
	
	TIM_DeInit(TIM6);
	TIM_TimeBaseInit(TIM6, &tim6); 
	
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM6,ENABLE);
}

void SetBitRate(int samprate)
{
	TIM6->ARR = 120000000L/samprate;// Set the Autoreload Register value
}

void TIM7_Init(TimerHandler handler)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM7_Handler = handler;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 10000-1;    
    TIM_TimeBaseStructure.TIM_Prescaler = 6000-1;             
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;             
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);  
    TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	NVIC_Config(TIM7_IRQn, 3);
	
	TIM_Cmd(TIM7, DISABLE);
}

void TIM7_Start(uint16_t ms)
{
	TIM7->CNT = 0;
	TIM7->ARR = ms*10-1;
	TIM_Cmd(TIM7, ENABLE);
}

void TIM7_Stop(void)
{
	TIM_Cmd(TIM7, DISABLE);
}

void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		TIM_Cmd(TIM7, DISABLE);
		TIM7_Handler();
	}
}

void TIM2_Init(uint16_t ms, TimerHandler handler)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM2_Handler = handler;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 10*ms-1;    
    TIM_TimeBaseStructure.TIM_Prescaler = 6000-1;             
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;             
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	NVIC_Config(TIM2_IRQn, 4);
	
	TIM_Cmd (TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		TIM2_Handler();
	}
}

void TIM3_Init(uint16_t ms, TimerHandler handler)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM3_Handler = handler;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 10*ms-1;    
    TIM_TimeBaseStructure.TIM_Prescaler = 6000-1;             
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;             
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	NVIC_Config(TIM3_IRQn, 5);
	
	TIM_Cmd (TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM3_Handler();
	}
}

