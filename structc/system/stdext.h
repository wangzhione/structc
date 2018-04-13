#ifndef _H_STDEXIT
#define _H_STDEXIT

//
// stdxxx.h 扩展库
// 
// 核心功能 : 高效内存分配模型
//

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

//
// free_ - free 包装函数
// ptr      : 通过下面函数分配的内存, 必须通过这个函数销毁
// return   : void
//
extern void free_(void * ptr);

//
// malloc_ - malloc 包装, 封装一些业务特性代码
// size     : 分配的内存字节
// return   : 返回可使用的内存地址.
//
extern void * malloc_(size_t size);

//
// strdup_ - strdup 包装函数
// s        : '\0' 结尾 C 字符串
// return   : 拷贝后新的 C 字符串
//
extern char * strdup_(const char * s);

//
// calloc_ - calloc 包装, 封装一些特殊业务
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并且置0
//
extern void * calloc_(size_t num, size_t size);

//
// realloc_ - realoc 包装函数, 封装一些特殊业务
// ptr      : 重新分配的内存首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配好的新地址内容
//
extern void * realloc_(void * ptr, size_t size);

//
// 开启全局配置 free / malloc 功能
//
#if !defined(_NO_STDEXT_)

#   undef  free
#   undef  strdup
#   undef  malloc
#   undef  calloc
#   undef  realloc

#   define free    free_
#   define strdup  strdup_
#   define malloc  malloc_
#   define calloc  calloc_
#   define realloc realloc_

#endif

#endif//_H_STDEXIT
