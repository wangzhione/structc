#ifndef _H_STDEXIT
#define _H_STDEXIT

/*
 stdxxx.h 扩展库

 主要功能 : 扩展内存分配

 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//
// free_ext - free 包装函数
// ptr      : 通过下面函数分配的内存, 必须通过这个函数销毁
// return   : void
//
extern void free_ext(void * ptr);

//
// malloc_ext - malloc 包装, 封装一些业务特性代码
// size     : 分配的内存字节
// return   : 返回可使用的内存地址.
//
extern void * malloc_ext(size_t size);

//
// calloc_ext - calloc 包装, 封装一些特殊业务
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并且置0
//
extern void * calloc_ext(size_t num, size_t size);

//
// realloc_ext - realoc 包装函数, 封装一些特殊业务
// ptr      : 重新分配的内存首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配好的新地址内容
//
extern void * realloc_ext(void * ptr, size_t size);

//
// 开启全局配置 free / malloc 功能
//
#if !defined(_NO_STDEXT_)

#   undef  free
#   undef  malloc
#   undef  calloc
#   undef  realloc

#   define free    free_ext
#   define malloc  malloc_ext
#   define calloc  calloc_ext
#   define realloc realloc_ext

#endif

#endif//_H_STDEXIT
