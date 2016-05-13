#include "xmodem.h"
#include "crc16.h"
//#include "printf.h"

int32_t last_error = 0;
//1024 for XModem 1k + 3 head chars + 2 crc + nul 
uint8_t xm_buf[1030]; 

void xm_outbyte(uint8_t ch)
{
	xm_port_write(ch);
}

uint8_t xm_inbyte(uint16_t time_out)
{
	uint8_t ch = 0;

	last_error = 0;

	xm_timer_start(time_out);
	while(!xm_is_timeout()){
		if(xm_port_read(&ch))
			return ch;
	}
	xm_timer_stop();

	last_error = 1;
	return ch;
}

void *xm_memcpy(void *destaddr, void const *srcaddr, int32_t len)
{
  	uint8_t *dest = destaddr;
  	uint8_t const *src = srcaddr;

  	while(len-- > 0)
   		*dest++ = *src++ ;
  	return destaddr;
}

void *xm_memset(void *src, int32_t c, int32_t count)
{
	 uint8_t *tmpsrc=(uint8_t*)src;
	 while(count--)
	  	*tmpsrc++ =(uint8_t)c;
	 return src;
}

static int32_t xm_check(int32_t crc, const uint8_t *buf, int32_t sz)
{
	if (crc) 
	{
		uint16_t crc = xm_crc16_ccitt(buf, sz);
		uint16_t tcrc = (buf[sz]<<8)+buf[sz+1];
		if (crc == tcrc)
			return 1;
	}
	else 
	{
		int32_t i;
		uint8_t cks = 0;
		for (i = 0; i < sz; ++i) 
		{
			cks += buf[i];
		}
		if (cks == buf[sz])
		return 1;
	}

	return 0;
}

//int32_t xm_receive(uint8_t *dest, int32_t destsz)
int32_t xm_receive(void)
{
	uint8_t *p;
	int32_t bufsz, crc = 0;
	uint8_t trychar = 'C';
	uint8_t packetno = 1;
	int32_t i, c, len = 0;
	int32_t retry, retrans = MAXRETRANS;

	for(;;) 
	{
		for(retry=0; retry<XM_RETRY; ++retry) 
		{
			if (trychar) 
				xm_outbyte(trychar);
			c = xm_inbyte(2000);
			if (last_error == 0) 
			{
				switch (c) 
				{
					case XM_SOH:
						bufsz = 128;
						goto start_recv;
					case XM_STX:
						bufsz = 1024;
						goto start_recv;
					case XM_EOT:
						xm_outbyte(XM_ACK);
						return len; /* normal end */
					case XM_CAN:
						c = xm_inbyte(1000);
						if (c == XM_CAN) 
						{
							xm_outbyte(XM_ACK);
							return -1; /* canceled by remote */
						}
						break;
					default:
						break;
				}
			}
		}
		if (trychar == 'C') 
		{ 
			trychar = XM_NAK; 
			continue; 
		}
		xm_outbyte(XM_CAN);
		xm_outbyte(XM_CAN);
		xm_outbyte(XM_CAN);
		return -2; /* sync error */

	start_recv:
		if (trychar == 'C') crc = 1;
		trychar = 0;
		p = xm_buf;
		*p++ = c;
		for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) 
		{
			c = xm_inbyte(1000);
			if (last_error != 0) 
				goto reject;
			*p++ = c;
		}

		if (xm_buf[1] == (uint8_t)(~xm_buf[2]) && (xm_buf[1] == packetno || xm_buf[1] == (uint8_t)packetno-1) && xm_check(crc, &xm_buf[3], bufsz)) 
		{
			if (xm_buf[1] == packetno)
			{
				len += bufsz;
				xm_frameReady(&xm_buf[3], bufsz);
				++packetno;
				retrans = MAXRETRANS+1;
			}
			if (--retrans <= 0) 
			{
				xm_outbyte(XM_CAN);
				xm_outbyte(XM_CAN);
				xm_outbyte(XM_CAN);
				return -3; /* too many retry error */
			}
			xm_outbyte(XM_ACK);
			continue;
		}
	reject:
		xm_outbyte(XM_NAK);
	}
}

int32_t xm_transmit(uint8_t *src, int32_t srcsz)
{
	uint8_t xm_buf[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	int32_t bufsz, crc = -1;
	uint8_t packetno = 1;
	int32_t i, c, len = 0;
	int32_t retry;

	for(;;) {
		for( retry = 0; retry < 16; ++retry) 
		{
			c = xm_inbyte(2000);
			if (last_error == 0) 
			{
				switch (c) 
				{
					case 'C':
						crc = 1;
						goto start_trans;
					case XM_NAK:
						crc = 0;
						goto start_trans;
					case XM_CAN:
						c = xm_inbyte(1000);
						if (c == XM_CAN) 
						{
							xm_outbyte(XM_ACK);
							return -1; /* canceled by remote */
						}
						break;
					default:
						break;
				}
			}
		}
		xm_outbyte(XM_CAN);
		xm_outbyte(XM_CAN);
		xm_outbyte(XM_CAN);
		return -2; /* no sync */

		for(;;) 
		{
		start_trans:
			xm_buf[0] = XM_SOH; bufsz = 128;
			xm_buf[1] = packetno;
			xm_buf[2] = ~packetno;
			c = srcsz - len;
			if (c > bufsz) c = bufsz;
			if (c >= 0) 
			{
				xm_memset (&xm_buf[3], 0, bufsz);
				if (c == 0) 
				{
					xm_buf[3] = XM_EOF;
				}
				else 
				{
					xm_memcpy (&xm_buf[3], &src[len], c);
					if (c < bufsz) xm_buf[3+c] = XM_EOF;
				}
				if (crc) 
				{
					uint16_t ccrc = xm_crc16_ccitt(&xm_buf[3], bufsz);
					xm_buf[bufsz+3] = (ccrc>>8) & 0xFF;
					xm_buf[bufsz+4] = ccrc & 0xFF;
				}
				else 
				{
					uint8_t ccks = 0;
					for (i = 3; i < bufsz+3; ++i) 
					{
						ccks += xm_buf[i];
					}
					xm_buf[bufsz+3] = ccks;
				}
				for (retry = 0; retry < MAXRETRANS; ++retry) 
				{
					for (i = 0; i < bufsz+4+(crc?1:0); ++i) 
					{
						xm_outbyte(xm_buf[i]);
					}
					c = xm_inbyte(1000);
					if (last_error == 0 ) 
					{
						switch (c) 
						{
							case XM_ACK:
								++packetno;
								len += bufsz;
								goto start_trans;
							case XM_CAN:
								c = xm_inbyte(1000);
								if ( c == XM_CAN) 
								{
									xm_outbyte(XM_ACK);
									return -1; /* canceled by remote */
								}
								break;
							case XM_NAK:
							default:
								break;
						}
					}
				}
				xm_outbyte(XM_CAN);
				xm_outbyte(XM_CAN);
				xm_outbyte(XM_CAN);
				return -4; /* xmit error */
			}
			else 
			{
				for (retry = 0; retry < 10; ++retry) 
				{
					xm_outbyte(XM_EOT);
					c = xm_inbyte(2000);
					if (c == XM_ACK) break;
				}
				return (c == XM_ACK)?len:-5;
			}
		}
	}
}



