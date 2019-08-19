#ifndef _UTF8_H
#define _UTF8_H

#include "struct.h"

//
// utf8 和 gbk 基础能力处理库
//
// g = gbk 是 ascii 扩展码, u8 = utf8
// 2 * LEN(g) >= LEN(u8) >= LEN(g)
//

//
// u82g - utf8 to gbk save d mem
// g2u8 - gbk to utf8 save d mem by size n
// d        : mem
// n        : size
// return   : void
//
extern void u82g(char d[]);
extern void g2u8(char d[], size_t n);

//
// isu8s - 判断字符串是否是utf8编码
// s        : 输入的串
// return   : true 表示 utf8 编码
//
extern bool isu8s(const char * s);

//
// isu8 - check is utf8
// d        : mem
// n        : size
// return   : true 表示 utf8 编码
//
extern bool isu8(const char d[], size_t n);

#endif//_UTF8_H
