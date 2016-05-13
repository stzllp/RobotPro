#include "usart.h"
#include "util.h"
#include "gpio.h"
#include <stdio.h>

SerialHandler UART4_Handler;
SerialHandler USART1_Handler;
SerialHandler USART2_Handler;

void USART1_Init(uint32_t baudRate, SerialHandler handler)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_AfPpUpOut(GPIOA, GPIO_Pin_9|GPIO_Pin_10);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	
	if(handler != NULL){
		USART1_Handler = handler;
		USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);	
		NVIC_Config(USART1_IRQn, 1);
	}
}

void USART1_HalfDuplexInit(uint32_t baudRate, SerialHandler handler)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_AfPpUpOut(GPIOB, GPIO_Pin_6|GPIO_Pin_7);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
	
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_HalfDuplexCmd(USART1, ENABLE);
	USART_Cmd(USART1, ENABLE);
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	
	if(handler != NULL){
		USART1_Handler = handler;
		USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);	
		NVIC_Config(USART1_IRQn, 1);
	}
}

void USART2_Init(uint32_t baudRate, SerialHandler handler)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_AfPpUpOut(GPIOA, GPIO_Pin_2|GPIO_Pin_3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	USART_GetFlagStatus(USART2, USART_FLAG_TC);
	
	if(handler != NULL){
		USART2_Handler = handler;
		USART_ITConfig(USART2, USART_IT_RXNE,ENABLE);	
		NVIC_Config(USART2_IRQn, 6);
	}
}

void UART4_Init(uint32_t baudRate, SerialHandler handler)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_AfPpUpOut(GPIOC, GPIO_Pin_10|GPIO_Pin_11);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	USART_Cmd(UART4, ENABLE);
	USART_GetFlagStatus(UART4, USART_FLAG_TC);
	
	if(handler != NULL){
		UART4_Handler = handler;
		USART_ITConfig(UART4, USART_IT_RXNE,ENABLE);	
		NVIC_Config(UART4_IRQn, 2);
	}
}

void USART1_IRQHandler(void)
{
	uint8_t ch;
	if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		ch = USART_ReceiveData(USART1);
		USART1_Handler(ch);
	}
}

void USART2_IRQHandler(void)
{
	uint8_t ch;
	if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		ch = USART_ReceiveData(USART2);
		USART2_Handler(ch);
	}
}

void UART4_IRQHandler(void)
{
	uint8_t ch;
	if(USART_GetITStatus(UART4,USART_IT_RXNE)==SET)
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		ch = USART_ReceiveData(UART4);
		UART4_Handler(ch);
	}
}

void USART_SendByte(USART_TypeDef *usart, uint8_t val)
{
	USART_SendData(usart, val);
	while (USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET);
}

void USART_Send(USART_TypeDef *usart, uint8_t *buf, uint32_t len)
{
	uint32_t i;
	for(i=0; i<len; i++){
		USART_SendByte(usart, buf[i]);
	}
}

int8_t USART_ReadByte(USART_TypeDef *usart, uint8_t *ch)
{
	if(USART_GetFlagStatus(usart, USART_FLAG_RXNE)==SET)
	{
		USART_ClearFlag(usart, USART_FLAG_RXNE);
		*ch = USART_ReceiveData(usart);
		return 1;
	}
	return 0;
}

void USART_SetTxMode(USART_TypeDef *usart)
{
	usart->CR1 &= ~USART_Mode_Rx;
	usart->CR1 |= USART_Mode_Tx;
}

void USART_SetRxMode(USART_TypeDef *usart)
{
	usart->CR1 &= ~USART_Mode_Tx;
	usart->CR1 |= USART_Mode_Rx;
}


