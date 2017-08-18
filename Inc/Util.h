#ifndef _UTIL_H_
#define _UTIL_H_
#include "stm32f1xx_hal.h"
#define	_mi(x)					memset((void *)(x), 0, sizeof(*(x)))
#define	_si(x)					memset((x), 0, sizeof((x)))
#define	_bit(bit)					(1 << (bit))
#define	bit_set(n, bit)			((n) |= _bit(bit))
#define	bit_del(n, bit)			((n) &= (~(_bit(bit))))
#define	bit_is_set(n, bit)			((n) & _bit(bit))

BOOLEAN bufferCmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
BOOLEAN bufferCmp16(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
#endif
