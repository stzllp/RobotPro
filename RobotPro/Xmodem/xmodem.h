#ifndef _XMODEM_H_
#define _XMODEM_H_

#include "xmodem_hw.h"

#define XM_RETRY	16		//retry����

#define XM_SOH  	0x01	//Xmodem����ͷ
#define XM_STX  	0x02	//1K-xmodem����ͷ
#define XM_EOT  	0x04	//���ͽ���
#define XM_ACK  	0x06	//�Ͽ�Ӧ��
#define XM_NAK  	0x15	//���Ͽ�Ӧ��
#define XM_CAN  	0x18	//�Ի���������
#define XM_EOF 		0x1A	//���ݰ����

#define XM_OK		0
#define XM_ERR		-1

#define MAXRETRANS 	25

//����ʵ�ʽ��յ����ݳ���
int32_t xm_receive(void);
int32_t xm_transmit(uint8_t *src, int32_t srcsz);

#endif /* _CRC16_H_ */
