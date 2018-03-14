#ifndef _H_G2U8
#define _H_G2U8

#include <stddef.h>
#include <stdbool.h>

//
// g = gbk 是 ascii 扩展码, u8 = UTF-8
// LEN(gbk) * 2 + 1 > LEN(utf8) >= LEN(gbk)
//

//
// isu8 - 判断当前字符串是否是 utf-8 编码
// buf      : 输入的串
// sz       : 待检查串长度
// return   : true 表示 utf-8 编码
//
extern bool isu8s(char buf[]);
extern bool isu8(char buf[], size_t sz);

//
// g2u8 - gbk 串数组转成 utf-8 串数组
// buf      : gbk 字符数组
// out      : utf-8 保存的数组
// sz       : buf 能够保存字符串容量
// return   : void
//
extern void g2u8(char buf[], char out[]);
extern void g2u8s(char buf[], size_t sz);

//
// g2u8 - utf-8 串数组转成 gbk 串数组
// buf      : utf-8 字符数组
// out      : gbk 保存的数组
// sz       : buf 能够保存字符串容量
// return   : void
//
extern void u82g(char buf[], char out[]);
extern void u82gs(char buf[]);

#endif//_H_G2U8
