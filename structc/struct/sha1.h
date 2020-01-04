#pragma once

#include <stdio.h>
#include <string.h>
#include "system.h"

struct sha1 {
    uint8_t buffer[64];     // 64 byte * 8 = 512 bit 消息块
    uint32_t state[ 5];     // 32 byte * 5 = 160 bit 消息摘要
    uint32_t count[ 2];     // 消息 bit 级别的长度, 1 是高位
};

//
// reference redis sha1 code
// sha1 20 byte, 默认转为 2 * 20 + 1 = 40 + 1 位的 C 串摘要
//
typedef uint8_t sha1_t[40 + 1];

extern uint8_t * sha1_file(sha1_t a, const char * path);
extern uint8_t * sha1_data(sha1_t a, const void * data, size_t n);

//
// sha1_str - 得到 40 位的 sha1 摘要串
// a        : 存储 sha1_t 串地址
// str      : C 字符串
// return   : m 首地址
//
inline uint8_t * sha1_str(sha1_t a, const char * str) {
    return sha1_data(a, str, strlen(str));
}
