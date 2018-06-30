#define OFF_ALLOC
#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jemalloc/jemalloc.h>

//
// free_ - free 包装函数
// ptr      : 内存首地址
// return   : void
//
inline void free_(void * ptr) {
    je_free(ptr);
}

// 简单内存不足检测处理
inline void * mcheck(void * ptr, size_t size) {
    if (NULL == ptr) {
        fprintf(stderr, "out of memory trying to allocate %zu\n", size);
        fflush(stderr);
        abort();
    }
    return ptr;
}

//
// malloc_ - malloc 包装, 封装一些业务特性代码
// size     : 分配的内存字节
// return   : 返回可使用的内存地址.
//
inline void * malloc_(size_t size) {
    void * ptr = je_malloc(size);
    return mcheck(ptr, size);
}

//
// strdup_ - strdup 包装函数
// s        : '\0' 结尾 C 字符串
// return   : 拷贝后新的 C 字符串
//
inline char * strdup_(const char * s) {
    if (s) {
        size_t n = strlen(s) + 1;
        char * ptr = malloc_(n);
        return memcpy(ptr, s, n);
    }
    return NULL;
}

//
// calloc_ - calloc 包装, 封装一些特殊业务
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并且置0
//
inline void * calloc_(size_t num, size_t size) {
    void * ptr = je_calloc(num, size);
    return mcheck(ptr, size);
}

//
// realloc_ - realoc 包装函数, 封装一些特殊业务
// ptr      : 内存首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配好的新地址内容
//
inline void * realloc_(void * ptr, size_t size) {
    void * ntr = je_realloc(ptr, size);
    return mcheck(ntr, size);
}
