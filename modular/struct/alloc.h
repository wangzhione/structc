#pragma once

//
// 这是个非常简单粗暴内存分配模块. 
// 多数这类模块会和项目业务绑定, 例如添加栈日志打印.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// check 内存检测并处理
inline void * check(void * ptr, size_t size) {
    if (!ptr) {
        fprintf(stderr, "check memory collapse %zu\n", size);
        fflush (stderr);
        abort();
    }
    return ptr;
}

//
// malloc_  - malloc 包装函数
// size     : 分配的内存字节
// return   : 返回可使用的内存地址
//
inline void * malloc_(size_t size) {
    return check(malloc(size), size);
}

//
// strdup_  - strdup 包装函数
// str      : '\0' 结尾 C 字符串
// return   : 拷贝后新的 C 字符串
//
inline char * strdup_(const char * str) {
    if (str) {
        size_t n = strlen(str)+1;
        return memcpy(malloc_(n), str, n);
    }
    return NULL;
}

//
// calloc_  - calloc 包装函数
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并置 0
//
inline void * calloc_(size_t num, size_t size) {
    return check(calloc(num, size), size);
}

//
// realloc_ - realoc 包装函数
// ptr      : 首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配的新地址
//
inline void * realloc_(void * ptr, size_t size) {
    return check(realloc(ptr, size), size);
}

// :) 包裹内存分配层, 些许感怀 ~ 可以通过 define ALLOC_OFF 关闭
//
#ifndef ALLOC_OFF
#  undef    malloc
#  define   malloc  malloc_
#  undef    strdup
#  define   strdup  strdup_
#  undef    calloc
#  define   calloc  calloc_
#  undef    realloc
#  define   realloc realloc_
#endif
