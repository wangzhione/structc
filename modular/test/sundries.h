﻿#pragma once

#include <ctype.h>
#include <stdlib.h>

/*
  这类编码库被放弃, 学习意义可以具体参照 cURL 顶级网络工具库
 */

//
// is_email - 判断是否是邮箱
// mail     : email 串
// return   : true is email
//
extern bool is_email(const char *mail);

//
// URL 编码做了如下操作：
// 字符 [a-z] [A-Z] [0-9] [&-./:=?_]都不被编码，维持原值；
// 空格" "被转换为加号"+"。
// 其他每个字节都被表示成 "%xy" 格式的由3个字符组成的字符串，编码为 UTF-8。
//
// ('a' <= c && c <= 'z')
// ('A' <= c && c <= 'Z')
// ('0' <= c && c <= '9')
// ( c != '&' && c != '-' && c != '.' && c != '/' && c != ':' && c != '&' && c != '?' && c != '_')

//
// url_encode - url 编码, 需要自己 free
// s        : url 串
// len      : url 串长度
// nen      : 返回编码后串长度
// return   : 返回编码后串的首地址
//
extern char *url_encode(const char *s, int len, int *nen);

//
// url_decode - url 解码, 解码后也是放在 s[] 中
// s        : 待解码的串
// len      : 解码串长度
// return   : 返回解码串的长度, < 0 表示失败
//
extern int url_decode(char s[], int len);