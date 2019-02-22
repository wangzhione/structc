#ifndef _ALLOC_H
#define _ALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// enum 状态 >= 0 is Success, < 0 is Error
//
enum {
    SBase       =   +0, // 正确基础类型

    EBase       =   -1, // 错误基础类型
    EParam      =   -2, // 输入参数错误
    EFd         =   -3, // 文件打开失败
    EClose      =   -4, // 文件操作关闭
    EAccess     =   -5, // 没有操作权限
    EAlloc      =   -6, // 内存操作错误
    EParse      =   -7, // 协议解析错误
    EBig        =   -8, // 过大基础错误
    ESmall      =   -9, // 过小基础错误
    ETimeout    =  -10, // 操作超时错误
};

//
// realloc_ - realoc 包装函数
// ptr      : 首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配的新地址
//
extern void * realloc_(void * ptr, size_t size);

//
// calloc_  - calloc 包装函数
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并置 0
//
extern void * calloc_(size_t num, size_t size);

//
// malloc_  - malloc 包装函数
// size     : 分配的内存字节
// return   : 返回可使用的内存地址
//
extern void * malloc_(size_t size);

//
// free_    - free 包装函数
// ptr      : 内存首地址
// return   : void
//
extern void free_(void * ptr);

//
// strdup_  - strdup 包装函数
// s        : '\0' 结尾 C 字符串
// return   : 拷贝后新的 C 字符串
//
extern char * strdup_(const char * s);

// :) 内存分配包装层, 莫名伤感 ~
//
#  ifndef ALLOC_OFF
#    undef  free
#    define free      free_

#    undef  strdup
#    define strdup    strdup_

#    undef  malloc
#    define malloc    malloc_
#    undef  calloc
#    define calloc    calloc_
#    undef  realloc
#    define realloc   realloc_
#  endif//ALLOC_OFF

#endif//_ALLOC_H
