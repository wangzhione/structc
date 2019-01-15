#ifndef _STREXT_H
#define _STREXT_H

/*
    继承 : string.h
    功能 : 扩展 string.h 中部分功能, 方便业务层调用
 */

#include "alloc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

//
// str_hash - Brian Kernighan 与 Dennis Ritchie hash 算法
// str      : 字符串内容
// return   : 返回计算后的 hash 值
//
extern unsigned str_hash(const char * str);

//
// str_cpyn - tar 复制内容到 src 中
// src      : 返回保存内容
// tar      : 目标内容
// n        : 最大容量
// return   : 返回字符串长度
//
extern int str_cpyn(char * src, const char * tar, size_t n);

//
// str_cmpi - 字符串不区分大小写比较函数
// ls       : 左串
// rs       : 右串
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
extern int str_cmpi(const char * ls, const char * rs);

//
// str_cmpin - 字符串不区分小写的限定字符比较函数
// ls       : 左串
// rs       : 右串
// n        : 长度
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
extern int str_cmpin(const char * ls, const char * rs, size_t n);

//
// str_trim - 去除字符数组前后控制字符
// str      : 待操作的字符数组 \0 结尾
// return   : 返回构建好字符数组首地址
//
extern char * str_trim(char str[]);

//
// str_printf - 字符串构建函数
// format   : 构建格式参照 printf
// ...      : 参数集
// return   : char * 堆上内存
//
extern char * str_printf(const char * format, ...);

//
// str_freads - 读取整个文件内容返回, 需要事后 free
// path     : 文件路径
// return   : 文件内容字符串, NULL 表示读取失败
//
extern char * str_freads(const char * path);

//
// str_fwrites - 将 C 串 str 覆盖写到 path 文件中
// path     : 文件路径
// str      : C 串内容
// return   : >=0 is success, < 0 is error
//
extern int str_fwrites(const char * path, const char * str);

//
// str_fappends - 将 C 串 str 追加写到 path 文件末尾
// path     : 文件路径
// str      : C 串内容
// return   : >=0 is success, < 0 is error
//
extern int str_fappends(const char * path, const char * str);

#endif//_STREXT_H
