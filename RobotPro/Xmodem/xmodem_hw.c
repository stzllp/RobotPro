#include "xmodem_hw.h"
#include "timer.h"
#include "usart.h"
#include <stdio.h>

void xm_timeOutHandler(void);
uint16_t timeOutFlag = 0;

void xm_init(void)
{
	UART4_Init(921600, NULL);
	TIM7_Init(xm_timeOutHandler);
}

void xm_timer_start(uint16_t ms)
{
	timeOutFlag = 0;
	TIM7_Start(ms);
}

void xm_timer_stop(void)
{
	TIM7_Stop();
}

uint8_t xm_is_timeout(void)
{
	return timeOutFlag;
}

//串口收发，实用查询方式。
void xm_port_write(uint8_t ch)
{
	USART_SendByte(UART4, ch);
}

//串口接收函数，需要移植
int8_t xm_port_read(uint8_t *p)
{
	return USART_ReadByte(UART4, p);
}

void xm_timeOutHandler(void)
{
	timeOutFlag = 1;
	TIM7_Stop();
}

void xm_frameReady(uint8_t *buf, uint32_t len)
{
	static uint32_t res = 0;
	res += len;
	printf("%d\r\n", res);
//	uint32_t i;
//	for(i=0; i<len; i++){
//		printf("%02x ", buf[i]);
//	}
}



