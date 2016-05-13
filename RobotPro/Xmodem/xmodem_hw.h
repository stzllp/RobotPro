#ifndef _XMODEM_HW_H_
#define _XMODEM_HW_H_

#include <stdint.h>

void xm_init(void);
void xm_port_write(uint8_t ch);
int8_t xm_port_read(uint8_t *p);

void xm_timer_start(uint16_t ms);
void xm_timer_stop(void);
uint8_t xm_is_timeout(void);
void xm_frameReady(uint8_t *buf, uint32_t len);

#endif /* _CRC16_H_ */
