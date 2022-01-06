#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

//
// md5 16 byte, 默认转为 32 位的 C 串
//
typedef uint8_t md5_t[32 + 1];

extern uint8_t * md5_file(md5_t m, const char * path);
extern uint8_t * md5_data(md5_t m, const void * data, size_t n);

//
// md5_strs - 得到 32 位的 md5 串
// m        : 存储 md5 串地址
// str      : C 字符串
// return   : m 首地址
//
inline uint8_t * md5_str(md5_t m, const char * str) {
    return md5_data(m, str, strlen(str));
}
