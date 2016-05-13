#include "usb_delay.h"

void usb_delay_us(uint32_t x)
{
	uint32_t i,j; // volatile is to avoid compiler optimization
	i = x;
	while(i--)
	{
		for(j=28;j>0;j--);
	}
}

void usb_delay_ms(uint32_t x)
{
	uint32_t i,j; // volatile is to avoid compiler optimization
	i = x;
	while(i--)
	{
		for(j=30000;j>0;j--);
	}
}

void usb_delay_s(uint32_t x)
{
	uint32_t i,j; // volatile is to avoid compiler optimization
	i = x;
	while(i--)
	{
		for(j=30100000;j>0;j--);
	}
}

