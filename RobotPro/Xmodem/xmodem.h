#ifndef _XMODEM_H_
#define _XMODEM_H_

#include "xmodem_hw.h"

#define XM_RETRY	16		//retry次数

#define XM_SOH  	0x01	//Xmodem数据头
#define XM_STX  	0x02	//1K-xmodem数据头
#define XM_EOT  	0x04	//发送结束
#define XM_ACK  	0x06	//认可应答
#define XM_NAK  	0x15	//不认可应答
#define XM_CAN  	0x18	//丛机撤销传输
#define XM_EOF 		0x1A	//数据包填充

#define XM_OK		0
#define XM_ERR		-1

#define MAXRETRANS 	25

//返回实际接收的数据长度
int32_t xm_receive(void);
int32_t xm_transmit(uint8_t *src, int32_t srcsz);

#endif /* _CRC16_H_ */
