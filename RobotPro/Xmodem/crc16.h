#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>

uint16_t xm_crc16_ccitt(const uint8_t *buf, int32_t len);

#endif /* _CRC16_H_ */
