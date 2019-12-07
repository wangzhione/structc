#ifndef __STRUCTC_STRUCT_CRC_H
#define __STRUCTC_STRUCT_CRC_H

#include <stdint.h>
#include <stddef.h>

extern uint16_t crc16(const void * data, size_t n);
extern uint64_t crc64(const void * data, size_t n);

#endif//__STRUCTC_STRUCT_CRC_H
