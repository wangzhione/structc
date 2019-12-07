#include "crc$table.h"

uint16_t 
crc16(const void * data, size_t n) {
    uint16_t crc = 0;
    for (const uint8_t * p = data; n-- > 0; )
        crc = crc << 8 ^ crc16_table[crc >> 8 ^ *p++];        
    return crc;
}

uint64_t 
crc64(const void * data, size_t n) {
    uint64_t crc = 0;
    for (const uint8_t * p = data; n-- > 0; )
        crc = crc64_table[(uint8_t)crc ^ *p++] ^ crc >> 8;
    return crc;
}

uint64_t crc64_update(uint64_t crc, const uint8_t * p, size_t n) {
    while (n-- > 0) 
        crc = crc64_table[(uint8_t)crc ^ *p++] ^ crc >> 8;
    return crc;
}
