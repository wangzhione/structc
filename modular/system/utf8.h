#pragma once

#include "struct.h"

//
// utf8 和 gbk 基础能力处理库
//
// g = gbk 是 ascii 扩展码, u8 = utf8
// 2 * LEN(g) >= LEN(u8) >= LEN(g)
//
// 编码相关小知识科普: https://madmalls.com/blog/post/unicode-and-utf8/

//
// utf82gbk - utf8 to gbk save d mem
// gbk2utf8 - gbk to utf8 save d mem by size n
// d        : mem
// n        : size
// return   : void
//
extern void utf82gbk(char d[]);
extern void gbk2utf8(char d[], size_t n);

//
// isutf8s - 判断字符串是否是utf8编码
// s        : 输入的串
// return   : true 表示 utf8 编码
//
extern bool isutf8s(void * s);

//
// isutf8 - check is utf8
// d        : mem
// n        : size
// return   : true 表示 utf8 编码
//
extern bool isutf8(const char d[], size_t n);
