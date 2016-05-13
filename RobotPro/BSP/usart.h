#ifndef __USART_H__
#define __USART_H__

#include "stm32f2xx.h"

typedef void(*SerialHandler)(uint8_t ch);

void USART1_Init(uint32_t baudRate, SerialHandler handler);
void USART1_HalfDuplexInit(uint32_t baudRate, SerialHandler handler);
void USART2_Init(uint32_t baudRate, SerialHandler handler);
void UART4_Init(uint32_t baudRate, SerialHandler handler);

void USART_SendByte(USART_TypeDef *usart, uint8_t val);
void USART_Send(USART_TypeDef *usart, uint8_t *buf, uint32_t len);
int8_t USART_ReadByte(USART_TypeDef *usart, uint8_t *ch);

void USART_SetTxMode(USART_TypeDef *usart);
void USART_SetRxMode(USART_TypeDef *usart);

#endif
