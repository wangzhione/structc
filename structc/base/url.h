#ifndef _URL_H
#define _URL_H

#include "alloc.h"
#include <ctype.h>

//
// URL编码做了如下操作： 
// 字符 [a-z] [A-Z] [0-9] [&-./:=?_]都不被编码，维持原值；
// 空格" "被转换为加号"+"。
// 其他每个字节都被表示成 "%xy" 格式的由3个字符组成的字符串，编码为UTF-8。
//
// ('a' <= c && c <= 'z')
// ('A' <= c && c <= 'Z')
// ('0' <= c && c <= '9')
// ( c != '&' && c != '-' && c != '.' && c != '/' && c != ':' && c != '&' && c != '?' && c != '_')

//
// url_encode - url 编码, 需要自己 free
// s        : url串
// len      : url串长度
// nen      : 返回编码后串长度
// return   : 返回编码后串的首地址
// 
extern char * url_encode(const char * s, int len, int * nen);

//
// url_decode - url 解码, 解码后也是放在 s[] 中
// s        : 待解码的串
// len      : 解码串长度
// return   : 返回解码串的长度, < 0 表示失败
//
extern int url_decode(char s[], int len);

#endif//_URL_H
