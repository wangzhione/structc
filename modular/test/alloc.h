#pragma once

//
// 这是个非常简单粗暴 allocation 内存分配模块.
// 多数这类模块会和项目业务绑定, 例如添加栈日志打印. 例如分配数据统计
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ation_check 内存检测并处理
inline void * ation_check(void * ptr, size_t size) {
    if (ptr == NULL) {
        fprintf(stderr, "check memory collapse %zu\n", size);
        fflush (stderr);
        abort();
    }
    return ptr;
}

//
// ation_malloc  - malloc 包装函数
// size     : 分配的内存字节
// return   : 返回可使用的内存地址
//
inline void * ation_malloc(size_t size) {
    return ation_check(malloc(size), size);
}

//
// ation_strdup  - strdup 包装函数
// str      : '\0' 结尾 C 字符串
// return   : 拷贝后新的 C 字符串
//
inline char * ation_strdup(const char * str) {
    if (str != NULL) {
        size_t n = strlen(str) + 1;
        return memcpy(ation_malloc(n), str, n);
    }
    return NULL;
}

inline char * ation_strndup(const char * str, size_t size) {
    if (str != NULL) {
        // @see https://stackoverflow.com/questions/66346502/which-is-most-standard-strnlen-or-strnlen-s/66347259#66347259
        // POSIX 标准真是良心. 好的标准往往容易有好的生态. 而不需要程序员和土匪似得东抢西偷.
        size_t n = strnlen(str, size);
        char * dup = ation_malloc(n+1);
        dup[n] = 0;
        return memcpy(dup, str, n);
    }
    return NULL;
}

//
// ation_calloc  - calloc 包装函数
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并置 0
//
inline void * ation_calloc(size_t num, size_t size) {
    return ation_check(calloc(num, size), size);
}

//
// ation_realloc - realoc 包装函数
// ptr      : 首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配的新地址
//
inline void * ation_realloc(void * ptr, size_t size) {
    return ation_check(realloc(ptr, size), size);
}

// :) 包裹内存分配层, 些许感怀 ~ 可以通过 define ALLOC_OFF 关闭
//
#ifndef ALLOC_OFF
#  undef    strdup
#  define   strdup      ation_strdup
#  undef    strndup
#  define   strndup     ation_strndup
#  undef    malloc
#  define   malloc      ation_malloc
#  undef    calloc
#  define   calloc      ation_calloc
#  undef    realloc
#  define   realloc     ation_realloc
#endif
