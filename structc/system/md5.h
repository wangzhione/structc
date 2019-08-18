﻿#ifndef _MD5_H
#define _MD5_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

//
// md5 16 byte, 默认转为 32 位的 C 串
//
typedef uint8_t md5_t[2 * 16 + 1];

//
// md5_strs - 得到字符串的 md5 串
// m        : 存储 md5 串地址
// p        : 文件的路径
// return   : m 首地址
//
extern uint8_t * md5_file(md5_t m, const char * p);

//
// md5_strs - 得到字符串的 md5 串
// m        : 存储 md5 串地址
// d        : 内存块数据
// n        : 内存块长度
// return   : m 首地址
//
extern uint8_t * md5_data(md5_t m, const void * d, size_t n);

//
// md5_strs - 得到 32 位的 md5 串
// m        : 存储 md5 串地址
// s        : C 字符串
// return   : m 首地址
//
inline uint8_t * md5_str(md5_t m, const char * s) {
    return md5_data(m, s, strlen(s));
}

#endif//_MD5_H
