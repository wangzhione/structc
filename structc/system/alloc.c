#define ALLOC_OFF
#include "alloc.h"

#define JEMALLOC_NO_DEMANGLE
#include <jemalloc/jemalloc.h>

// check 内存检测并处理
inline void * check(void * ptr, size_t size) {
    if (!ptr) {
        fprintf(stderr, "check memory collapse %zu\n", size);
        fflush(stderr);
        abort();
    }
    return ptr;
}

//
// realloc_ - realoc 包装函数
// ptr      : 首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配的新地址
//
inline void * realloc_(void * ptr, size_t size) {
    return check(je_realloc(ptr, size), size);
}

//
// calloc_  - calloc 包装函数
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并置 0
//
inline void * calloc_(size_t num, size_t size) {
    return check(je_calloc(num, size), size);
}

//
// malloc_  - malloc 包装函数
// size     : 分配的内存字节
// return   : 返回可使用的内存地址
//
inline void * malloc_(size_t size) {
    return check(je_malloc(size), size);
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
// free_    - free 包装函数
// ptr      : 内存首地址
// return   : void
//
inline void free_(void * ptr) {
    je_free(ptr);
}
